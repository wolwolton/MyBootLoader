#include <Uefi.h>
#include <Library/UefiLib.h>
#include  <Library/UefiBootServicesTableLib.h>
#include  <Library/PrintLib.h>

#include  <Protocol/LoadedImage.h>
#include  <Protocol/SimpleFileSystem.h>
#include  <Protocol/DiskIo2.h>
#include  <Protocol/BlockIo.h>
#include  <Guid/FileInfo.h>
#include  <Protocol/GraphicsOutput.h>

#include <elf.h>

#include "memory_map.hpp"

EFI_STATUS GetMemoryMap(struct MemoryMap* map) {
    Print(L"Called");
  if (map->buffer == NULL) {
    Print(L"EFI_BUFER_TOO_SMALL");
    return EFI_BUFFER_TOO_SMALL;
  }

  map->map_size = map->buffer_size;
  Print(L"Map size is : %d",map->map_size);
  return gBS->GetMemoryMap(
      &map->map_size,
      (EFI_MEMORY_DESCRIPTOR*)map->buffer,
      &map->map_key,
      &map->descriptor_size,
      &map->descriptor_version);
}

const CHAR16* GetMemoryTypeUnicode(EFI_MEMORY_TYPE type) {
  switch (type) {
    case EfiReservedMemoryType: return L"EfiReservedMemoryType";
    case EfiLoaderCode: return L"EfiLoaderCode";
    case EfiLoaderData: return L"EfiLoaderData";
    case EfiBootServicesCode: return L"EfiBootServicesCode";
    case EfiBootServicesData: return L"EfiBootServicesData";
    case EfiRuntimeServicesCode: return L"EfiRuntimeServicesCode";
    case EfiRuntimeServicesData: return L"EfiRuntimeServicesData";
    case EfiConventionalMemory: return L"EfiConventionalMemory";
    case EfiUnusableMemory: return L"EfiUnusableMemory";
    case EfiACPIReclaimMemory: return L"EfiACPIReclaimMemory";
    case EfiACPIMemoryNVS: return L"EfiACPIMemoryNVS";
    case EfiMemoryMappedIO: return L"EfiMemoryMappedIO";
    case EfiMemoryMappedIOPortSpace: return L"EfiMemoryMappedIOPortSpace";
    case EfiPalCode: return L"EfiPalCode";
    case EfiPersistentMemory: return L"EfiPersistentMemory";
    case EfiMaxMemoryType: return L"EfiMaxMemoryType";
    default: return L"InvalidMemoryType";
  }
}

EFI_STATUS SaveMemoryMap(struct MemoryMap* map, EFI_FILE_PROTOCOL* file) {
  EFI_STATUS status;
  CHAR8 buf[256];
  UINTN len;

  CHAR8* header =
    "Index, Type, Type(name), PhysicalStart, NumberOfPages, Attribute\n";
  len = AsciiStrLen(header);
  status = file->Write(file, &len, header);
  if (EFI_ERROR(status)) {
    return status;
  }

  Print(L"map->buffer = %08lx, map->map_size = %08lx\n",
      map->buffer, map->map_size);

  EFI_PHYSICAL_ADDRESS iter;
  int i;
  for (iter = (EFI_PHYSICAL_ADDRESS)map->buffer, i = 0;
       iter < (EFI_PHYSICAL_ADDRESS)map->buffer + map->map_size;
       iter += map->descriptor_size, i++) {
    EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)iter;
    len = AsciiSPrint(
        buf, sizeof(buf),
        "%u, %x, %-ls, %08lx, %lx, %lx\n",
        i, desc->Type, GetMemoryTypeUnicode(desc->Type),
        desc->PhysicalStart, desc->NumberOfPages,
        desc->Attribute & 0xffffflu);
    status = file->Write(file, &len, buf);
    if (EFI_ERROR(status)) {
      return status;
    }
  }
  return EFI_SUCCESS;
}

EFI_STATUS OpenRootDir(EFI_HANDLE image_handle, EFI_FILE_PROTOCOL** root){
    EFI_STATUS status;
    EFI_LOADED_IMAGE_PROTOCOL* loaded_image;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* fs;

    status = gBS->OpenProtocol(
        image_handle, 
        &gEfiLoadedImageProtocolGuid,
        (VOID**)&loaded_image,
        image_handle,
        NULL,
        EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
    if(EFI_ERROR(status)){
        return status;
    }

    status = gBS->OpenProtocol(
        loaded_image->DeviceHandle,
        &gEfiSimpleFileSystemProtocolGuid,
        (VOID**)&fs,
        image_handle,
        NULL,
        EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
    if(EFI_ERROR(status)){
        return status;
    }


    return fs->OpenVolume(fs,root);
}

void Debug(UINT64 st_adr){
    for(int i = 0; i< 10; i++){
        Print(L"ADDR:%02d : ",st_adr+i*8);
        UINT8 data = *(UINT8*)(st_adr + i * 8);
        Print(L"%02x   ",data);
        data = *(UINT8*)(st_adr + i * 8 + 1);
        Print(L"%02x   ",data);
        data = *(UINT8*)(st_adr + i * 8 + 2);
        Print(L"%02x   ",data);
        data = *(UINT8*)(st_adr + i * 8 + 3);
        Print(L"%02x    ",data);
        data = *(UINT8*)(st_adr + i * 8 + 4);
        Print(L"%02x   ",data);
        data = *(UINT8*)(st_adr + i * 8 + 5);
        Print(L"%02x   ",data);
        data = *(UINT8*)(st_adr + i * 8 + 6);
        Print(L"%02x   ",data);
        data = *(UINT8*)(st_adr + i * 8 + 7);
        Print(L"%02x\n",data);
    }
}

EFI_STATUS getEntryPoint(EFI_FILE_PROTOCOL* file, UINT64* addr){
    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
UefiMain (
IN EFI_HANDLE ImageHandle,
IN EFI_SYSTEM_TABLE *SystemTable
)
{
    EFI_STATUS status;
    /*
    CHAR8 memmap_buf[4096 * 10];
    struct MemoryMap memmap = {sizeof(memmap_buf), memmap_buf, 0, 0, 0, 0};
    Print(L"Getting MemoryMap\n");
    status = GetMemoryMap(&memmap);
    if(EFI_ERROR(status)){
        Print(L"Getting memory map is failed.");
    }
    */
    EFI_GRAPHICS_OUTPUT_PROTOCOL *GOP;
    gBS->LocateProtocol(&gEfiGraphicsOutputProtocolGuid, NULL, (void **)&GOP);
    EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE* f_mode = (EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE*)GOP->Mode;
    Print(L"FBB:%02X\n",(UINT64)f_mode->FrameBufferBase);
    Print(L"hr%02X\n",f_mode->Info->HorizontalResolution);
    Print(L"vr%02X\n",f_mode->Info->VerticalResolution);

    EFI_FILE_PROTOCOL* root_dir;
    Print(L"Root Opening...");
    status = OpenRootDir(ImageHandle, &root_dir);
    if(EFI_ERROR(status)){
        Print(L"Root Open Failed\n");
    }else{
        Print(L"Root Open Success\n");
    }
    
    EFI_FILE_PROTOCOL* elffile;
    CHAR16* file_name = L"\\enoshima.elf";
    status = root_dir->Open(root_dir, &elffile, file_name, EFI_FILE_MODE_READ, 0);
    
    if(EFI_ERROR(status)){
        Print(L"failed to open file '%s': %r\n",file_name,status);
        while(1);
        return 0;
    }
    UINT64 elf_file_addr=0x100000lu;
    UINTN num_pages = 4;
    UINT64 num_pages_size = num_pages*4096;
    status = gBS->AllocatePages(AllocateAddress, EfiLoaderData, num_pages, &elf_file_addr);
    if(EFI_ERROR(status)){
        Print(L"Allocate Failed : %r\n", status);
        while(1);
        return 0;
    }
    status = elffile->Read(elffile, &num_pages_size, (void*)elf_file_addr);
    Print(L"numpagesize is: %d\n",num_pages_size);
    if(EFI_ERROR(status)){
        Print(L"failed to read file '%s': %r\n",file_name,status);
        while(1);
        return 0;
    }
    status = elffile->Close(elffile);
    if(EFI_ERROR(status)){
        Print(L"failed to close : %r\n",status);
    }
    status = root_dir->Close(root_dir);
    if(EFI_ERROR(status)){
        Print(L"failed to close Root : %r\n",status);
    }
    Print(L"Load Success\n");

    Elf32_Ehdr* e_hd = (Elf32_Ehdr *)elf_file_addr;
    typedef int EntryPointType(void*, EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE*);
    EntryPointType* entry_point = (EntryPointType*)((UINT64)e_hd->e_entry);
    
    f_mode->Mode = 0;
    Print(L"MaxMode is:%02X",(UINT64)f_mode->MaxMode);
    /*
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL *p;
    UINT64 vr = f_mode->Info->VerticalResolution;
    UINT64 hr = f_mode->Info->HorizontalResolution;
    int x,y;
    for (y = 0; y < vr; y++) {
        for (x = 0; x < hr; x++) {
            p = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL*)((UINT64)f_mode->FrameBufferBase + (hr * y) + x);
            p->Red   = 0;
            p->Green = 255;
            p->Blue  = 255;
            p->Reserved = 0;
        }
    }
    while(1);
    */

    Print(L"f_mode is:%02X",(UINT64)f_mode);
    //Run OS
    entry_point(Print,f_mode);
    
    /*
    if(EFI_ERROR(status)){
        Print(L"failed to open file '%s': %r\n",file_name,status);
    }else{
        status = SaveMemoryMap(&memmap,memmapfile);

        if(EFI_ERROR(status)){
            Print(L"failed to save: %r\n",status);
        }
        status = memmapfile->Close(memmapfile);
        if(EFI_ERROR(status)){
            Print(L"failed to close : %r\n",status);
        }
        status = root_dir->Close(root_dir);
        if(EFI_ERROR(status)){
            Print(L"failed to close Root : %r\n",status);
        }
        
        Print(L"SUCCESS SAVE!!");
    }
    */
    while (1);
    return 0;
}
