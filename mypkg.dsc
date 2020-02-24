[Defines]
PLATFORM_NAME = MyBootLoader
PLATFORM_GUID = e3ac225d-74db-43b2-8aee-7bb1c8a050e7
PLATFORM_VERSION = 0.1
DSC_SPECIFICATION = 0x00010005
OUTPUT_DIRECTORY = Build/MyBootLoder$(ARCH)
SUPPORTED_ARCHITECTURES = IA32|X64
BUILD_TARGETS = DEBUG|RELEASE|NOOPT
DEFINE DEBUG_ENABLE_OUTPUT = FALSE
[LibraryClasses]
# Entry point
UefiApplicationEntryPoint|MdePkg/Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf
# Common Libraries
BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
BaseMemoryLib|MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
!if $(DEBUG_ENABLE_OUTPUT)
DebugLib|MdePkg/Library/UefiDebugLibConOut/UefiDebugLibConOut.inf
DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
!else ## DEBUG_ENABLE_OUTPUT
DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
!endif ## DEBUG_ENABLE_OUTPUT
UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
[Components]
MyBootLoader/mypkg.inf
