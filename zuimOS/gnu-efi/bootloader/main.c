#include <efi.h>
#include <efilib.h>
#include <elf.h>

typedef unsigned long long size_t;

//Estrutura para o framebuffer 
//Pode utilizar essa estrutura para armazenar qualquer informacao do framebuffer para um driver grafico
typedef struct{
    //Endereco base e tamanho do buffer
	void* BaseAddress;
	size_t BufferSize;
	//resolucao horizontal
	unsigned int Width; 
	//resolucao vertica
	unsigned int Height;
	unsigned int PixelsPerScanLine;
} Framebuffer;

#define PSF1_MAGIC0 0x36
#define PSF1_MAGIC1 0x04

typedef struct {
  unsigned char magic[2];
  unsigned char mode;
  unsigned char charsize;
} PSF1_HEADER;

typedef struct {
  PSF1_HEADER* psf1_Header;
  void* glyphBuffer;
} PSF1_FONT;


Framebuffer framebuffer;

//GOP - Graphic Output Protocol
//Usado para permitir que o kernel produza imagens na tela
//Nao possui aceleracao por hardware, por isso e meio lento
Framebuffer* InitializeGOP(){
	//EFI graphics output protocol
	EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    //Ponteiro para ele
	EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
	//Para checar se ta tudo ok
	EFI_STATUS status;
    
	//corrige a convenção de chamada para que tudo seja chamado corretamente
	status = uefi_call_wrapper(BS->LocateProtocol, 3, &gopGuid, NULL, (void**)&gop);
	if(EFI_ERROR(status)){
		Print(L"Unable to locate GOP\n\r");
		return NULL;
	} else{
		Print(L"GOP located\n\r");
	}
    
	//Passando para o Framebuffer as informacoes do GOP
	framebuffer.BaseAddress = (void*)gop->Mode->FrameBufferBase;
	framebuffer.BufferSize = gop->Mode->FrameBufferSize;
	framebuffer.Width = gop->Mode->Info->HorizontalResolution;
	framebuffer.Height = gop->Mode->Info->VerticalResolution;
	framebuffer.PixelsPerScanLine = gop->Mode->Info->PixelsPerScanLine;

    return &framebuffer;
}


/*
 @EFI_FILE Directory - EFI considera diretorios como arquivos, entao se quiser pegar um arquivo dentro de um diretorio tem que passar esse directory primeiro
 @CHAR16 - Arranjo de caracteres que define o caminho do sistema onde o EFI vai procurar pelo arquivo
 @EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable - Toda função precisa desses dois
*/
EFI_FILE* LoadFile(EFI_FILE* Directory, CHAR16* Path, EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable){
    EFI_FILE* LoadedFile;

    EFI_LOADED_IMAGE_PROTOCOL* LoadedImage; //Pegar a imagem carregada do sistema
    SystemTable->BootServices->HandleProtocol(ImageHandle, &gEfiLoadedImageProtocolGuid, (void**)&LoadedImage);

	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* FileSystem; //Pegar o sistema de arquivos
    SystemTable->BootServices->HandleProtocol(LoadedImage->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (void**)&FileSystem);
    
	//Checar se diretorio vazio, e se sim, setar para a raiz do sistema de arquivos
	if(Directory == NULL){
		FileSystem->OpenVolume(FileSystem, &Directory);
	}
    
	//Abrindo o diretorio e salvando o arquivo em LoadedFile 
	EFI_STATUS s = Directory->Open(Directory, &LoadedFile, Path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
    //Checar se salvou com sucesso
	if(s != EFI_SUCCESS){
		return NULL;
	} 
	return LoadedFile;
}

PSF1_FONT* LoadPSF1Font(EFI_FILE* Directory, CHAR16* Path, EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable){
	EFI_FILE* font = LoadFile(Directory, Path, ImageHandle, SystemTable);
	if(font == NULL) return NULL;

	PSF1_HEADER* fontHeader;
	SystemTable->BootServices->AllocatePool(EfiLoaderData, sizeof(PSF1_HEADER), (void**)&fontHeader);
	UINTN size = sizeof(PSF1_HEADER);
	font->Read(font, &size, fontHeader);

	if(fontHeader->magic[0] != PSF1_MAGIC0 || fontHeader->magic[1] != PSF1_MAGIC1){
		return NULL;
	}

	UINTN glyphBufferSize = fontHeader->charsize * 256;
	if(fontHeader->mode == 1){ //512 glyph mode
		glyphBufferSize = fontHeader->charsize * 512;
	}

	void* glyphBuffer;
	{
		font->SetPosition(font, sizeof(PSF1_HEADER));
		SystemTable->BootServices->AllocatePool(EfiLoaderData, glyphBufferSize, (void**)&glyphBuffer);
		font->Read(font, &glyphBufferSize, glyphBuffer);
	}

	PSF1_FONT* finishedFont;
	SystemTable->BootServices->AllocatePool(EfiLoaderData, sizeof(PSF1_FONT), (void**)&finishedFont);
	finishedFont->psf1_Header = fontHeader;
	finishedFont->glyphBuffer = glyphBuffer;
	return finishedFont; 
}


//Checar se o elf executavel esta correto
int memcmp(const void* aptr, const void* bptr, size_t n){
	const unsigned char* a = aptr, *b = bptr;
	//scan through the buffers of memory and the size of n
	for (size_t i = 0; i < n; i++){
		if(a[i] < b[i]) return -1;
		else if (a[i] > b[i]) return 1;
	}
	return 0; 
}

EFI_STATUS efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
	InitializeLib(ImageHandle, SystemTable); //Setup UEFI enviroment to use special commands
	Print(L"String string is working \n\r"); //Write on screen 
    
	//Identificar o arquivo elf
	EFI_FILE* Kernel = LoadFile(NULL, L"kernel.elf", ImageHandle, SystemTable);
	if(Kernel == NULL){
		Print(L"Could not load kernel \n\r");
	} else{
		Print(L"Kernel loaded successfully \n\r");
	}
    
	//Criar o header do arquivo elf
	Elf64_Ehdr header;
	{
		//Alocar memoria para esse header
		UINTN FileInfoSize;
		EFI_FILE_INFO* FileInfo;
		//Setar o fileinfosize para o tamanho necessario para o Kernel
        Kernel->GetInfo(Kernel, &gEfiFileInfoGuid, &FileInfoSize, NULL);
        SystemTable->BootServices->AllocatePool(EfiLoaderData, FileInfoSize, (void**)&FileInfo);
		Kernel->GetInfo(Kernel, &gEfiFileInfoGuid, &FileInfoSize, (void**)&FileInfo);
        
		//Entregar para ele as informacoes do header do kernel
		UINTN size = sizeof(header);
		//Ler a quantidade de bits em size no header
		Kernel->Read(Kernel, &size, &header); 
	}
    
	//Checar se ta tudo certo no header
	if(
		memcmp(&header.e_ident[EI_MAG0], ELFMAG, SELFMAG) != 0 ||
		header.e_ident[EI_CLASS] != ELFCLASS64 ||
		header.e_ident[EI_DATA] != ELFDATA2LSB ||
		header.e_type != ET_EXEC ||
		header.e_machine != EM_X86_64 ||
		header.e_version != EV_CURRENT
	){
		Print(L"Kernel format is bad \r\n");
	}else{
		Print(L"Kernel header successfully verified \r\n");
	}

    //Carregar as informacoes desse elf para a memoria (Program header)
	Elf64_Phdr* phdrs;
	{
		//Program header offset
		Kernel->SetPosition(Kernel, header.e_phoff);
		UINTN size = header.e_phnum * header.e_phentsize;
		//Alocar memoria para PH
		SystemTable->BootServices->AllocatePool(EfiLoaderData, size, (void**)&phdrs);
		//Pegar todos os bits do program header e colocar na estrutura
		Kernel->Read(Kernel, &size, phdrs);
	}
    
    //passar por todos os header e carregar a informação binaria
	for (
		Elf64_Phdr* phdr = phdrs;
		(char*)phdr < (char*)phdrs + header.e_phnum * header.e_phentsize;
		phdr = (Elf64_Phdr*)((char*)phdr + header.e_phentsize)
	){
		switch(phdr->p_type){
			case PT_LOAD:
			{    
			    //Alocar paginas para carregar o programa na memoria onde ele precisa estar
				int pages = (phdr->p_memsz + 0x1000 - 1) / 0x1000;
				Elf64_Addr segment = phdr->p_paddr;
				SystemTable->BootServices->AllocatePages(AllocateAddress, EfiLoaderData, pages, &segment);
                
				//Set the right offset to read from the kernel
				Kernel->SetPosition(Kernel, phdr->p_offset);
				UINTN size = phdr->p_filesz;
				Kernel->Read(Kernel, &size, (void*)segment);
				break;
			}
		}
	}

	Print(L"Kernel Loaded\n\r"); 
    
	void (*KernelStart)(Framebuffer*, PSF1_FONT*) = ((__attribute__((sysv_abi)) void(*)(Framebuffer*, PSF1_FONT*) ) header.e_entry);
    
    PSF1_FONT* newFont = LoadPSF1Font(NULL, L"zap-light16.psf", ImageHandle, SystemTable);

	if(newFont == NULL){
		Print(L"Font nao encontrado ou invalido\n\r");
	}else{
		Print(L"Font encontrado. char size = %d\n\r", newFont->psf1_Header->charsize);
	}

	Framebuffer* newBuffer = InitializeGOP();
	
	Print(L"Base: 0x%x\n\r Size: 0x%x\n\r Width: %d\n\r  Height: %d\n\r	PixelsPerScanLine: %d\n\r", newBuffer->BaseAddress, newBuffer->BufferSize, newBuffer->Width, newBuffer->Height, newBuffer->PixelsPerScanLine);

    KernelStart(newBuffer, newFont);

	return EFI_SUCCESS; // Exit the UEFI application
}
