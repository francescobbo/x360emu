#include <Windows.h>
#include <ntstatus.h>
#include "../Xenon/Xenon.h"
#include "../Memory.h"

/**
 * Remember that Xbox 360 library calls may have the same names of Win32
 * functions, but arguments aren't always the same and in the same order.
 * Some arguments might not be used, and others might have been added.
 */

namespace HLE
{
	enum MemState
	{
		Reserved, Committed, Free
	};

	struct MemEntry
	{
		MemEntry *prev, *next;
		u32 Base;
		u32 Size;
		MemState State;
	};

	MemEntry *entry = NULL;
	const u32 MemEntryMin = 0x90000000;
	const u32 MemEntryMax = 0x9FFFFFFF;

	/**
	 * NTSTATUS NtAllocateVirtualMemory(PVOID *BaseAddress, PSIZE_T RegionSize, int AllocationType, int Protect, int Unknown);
	 * We can safely ignore the @Protect argument, as it will be respected by the game itself (infact games work on the Xbox)
	 */
	void NtAllocateVirtualMemory(Xenon::CpuState *xState)
	{
		uint32_t BaseAddress = xState->gpr[3];
		uint32_t RegionSize = xState->gpr[4];
		uint32_t AllocationType = xState->gpr[5];
		uint32_t Unknown = xState->gpr[7];

		uint32_t RealBase = Memory::Read32(BaseAddress);
		uint32_t RealSize = Memory::Read32(RegionSize);

		if (RealBase)
		{
			if (RealBase + RealSize > MemEntryMax)
				throw "NtVirtualAlloc: Out Of Memory!!!";

			if (!entry)
			{
				/* If this is the first allocation, things are really easy */
				entry = new MemEntry;
				entry->next = entry->prev = NULL;
				entry->Base = RealBase;
				entry->Size = RealSize;
				if (AllocationType & MEM_COMMIT)
					entry->State = Committed;
				else if (AllocationType & MEM_RESERVE)
					entry->State = Reserved;
				else
					throw "NtVirtualAlloc: Unknown AllocationType";

				Memory::Write32(BaseAddress, MemEntryMin);
				xState->gpr[3] = STATUS_SUCCESS;
				return;
			}
			else
			{
				MemEntry *last = entry;
				while (entry)
				{
					if (entry->Base == RealBase)
					{
						if (AllocationType & MEM_COMMIT)
							entry->State = Committed;

						xState->gpr[3] = STATUS_SUCCESS;
						return;
					}
					else if (entry->Base > RealBase && RealBase + RealSize > entry->Base)
					{
						throw "NtVirtualAlloc: Overlapping zones!";
					}
					else if (entry->Base > RealBase)
					{
						if (entry->Base - (entry->prev->Base + entry->prev->Size) >= RealSize)
						{
							/* We found a hole between the previous and this, we can put our entry here */
							MemEntry *tmp = entry->prev;
							entry->prev = tmp->next = new MemEntry;
							entry->prev->next = entry;
							entry->prev->prev = tmp;

							/* Fill the fields */
							u32 newBase = (tmp->Base + tmp->Size + 0xFFF) & ~0xFFF;
							entry = entry->prev;
							entry->Base = newBase;
							entry->Size = RealSize;

							if (AllocationType & MEM_COMMIT)
								entry->State = Committed;
							else if (AllocationType & MEM_RESERVE)
								entry->State = Reserved;
							else
								throw "NtVirtualAlloc: Unknown AllocationType";

							xState->gpr[3] = STATUS_SUCCESS;
							return;
						}
					}

					last = entry;
					entry = entry->next;
				}

				/* Append a new entry */
				last->next = new MemEntry;
				last->next->prev = last;
				last->next->next = NULL;

				u32 newBase = (last->Base + last->Size + 0xFFF) & ~0xFFF;
				if (newBase + RealSize > MemEntryMax)
					throw "NtVirtualAlloc: Out of Memory!!!";

				entry = last->next;
				entry->Base = newBase;
				entry->Size = RealSize;

				if (AllocationType & MEM_COMMIT)
					entry->State = Committed;
				else if (AllocationType & MEM_RESERVE)
					entry->State = Reserved;
				else
					throw "NtVirtualAlloc: Unknown AllocationType";

				/* Report allocated Base address */
				Memory::Write32(BaseAddress, MemEntryMin);
				xState->gpr[3] = STATUS_SUCCESS;
				return;
			}
		}
		else
		{
			if (!entry)
			{
				/* If this is the first allocation, things are really easy */

				entry = new MemEntry;
				entry->next = entry->prev = NULL;
				entry->Base = MemEntryMin;
				entry->Size = RealSize;
				if (AllocationType & MEM_COMMIT)
					entry->State = Committed;
				else if (AllocationType & MEM_RESERVE)
					entry->State = Reserved;
				else
					throw "NtVirtualAlloc: Unknown AllocationType";

				Memory::Write32(BaseAddress, MemEntryMin);
				xState->gpr[3] = STATUS_SUCCESS;
				return;
			}
			else
			{
				/* Try to find a hole between two consecutive pieces */

				MemEntry *last = entry;
				while (entry)
				{
					u32 diff = entry->Base + entry->Size - entry->next->Base;
					if (diff >= RealSize)
					{
						/* Got the hole. Now create and link a new entry */
						MemEntry *tmp = entry->next;
						entry->next = tmp->prev = new MemEntry;
						entry->next->prev = entry;
						entry->next->next = tmp;

						/* Fill the fields */
						u32 newBase = (entry->Base + entry->Size + 0xFFF) & ~0xFFF;
						entry = entry->next;
						entry->Base = newBase;
						entry->Size = RealSize;

						if (AllocationType & MEM_COMMIT)
							entry->State = Committed;
						else if (AllocationType & MEM_RESERVE)
							entry->State = Reserved;
						else
							throw "NtVirtualAlloc: Unknown AllocationType";

						/* Report allocated Base address */
						Memory::Write32(BaseAddress, MemEntryMin);
						xState->gpr[3] = STATUS_SUCCESS;
						return;
					}

					last = entry;
					entry = entry->next;
				}

				/* No space has been found between various entries. Time to append a new one */
				last->next = new MemEntry;
				last->next->prev = last;
				last->next->next = NULL;

				u32 newBase = (last->Base + last->Size + 0xFFF) & ~0xFFF;
				if (newBase + RealSize > MemEntryMax)
					throw "NtVirtualAlloc: Out of Memory!!!";

				entry = last->next;
				entry->Base = newBase;
				entry->Size = RealSize;

				if (AllocationType & MEM_COMMIT)
					entry->State = Committed;
				else if (AllocationType & MEM_RESERVE)
					entry->State = Reserved;
				else
					throw "NtVirtualAlloc: Unknown AllocationType";

				/* Report allocated Base address */
				Memory::Write32(BaseAddress, MemEntryMin);
				xState->gpr[3] = STATUS_SUCCESS;
				return;
			}
		}
	}

	/* NTSTATUS NtFreeVirtualMemory(PVOID *BaseAddress, PSIZE_T RegionSize, int FreeType, int Unknown); */
	void NtFreeVirtualMemory(Xenon::CpuState *xState)
	{
		u32 BaseAddress = (u32) xState->gpr[3];
		u32 RegionSize = (u32) xState->gpr[4];
		u64 FreeType = xState->gpr[5];
		u64 Unknown = xState->gpr[6];

		uint32_t RealSize = Memory::Read32(RegionSize);

		/* Well... let's try... */
		xState->gpr[3] = STATUS_SUCCESS;
	}
}
