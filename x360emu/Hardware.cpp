/**
 * x360emu - An emulator for the Xbox 360 gaming system.
 * Copyright (C) 2012 - The x360emu Project
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

#include "Hardware.h"
#include "Memory.h"
#include "Xenon/Xenon.h"

namespace Hardware
{
    void Init()
    {
        Memory::Init();
        Xenon::Init();
    }

    void Shutdown()
    {
        Memory::Shutdown();
    }

    void Boot()
    {
        Xenon::Boot();
    }
}
