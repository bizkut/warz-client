#ifndef	__R3D_BINFMT_H
#define	__R3D_BINFMT_H

#define R3D_BINARY_FILE_ID	'2d3r'

#define R3D_DEFAULT_BINARY_HDR			\
	  DWORD		r3dID;			\
	  DWORD		ID;                     \
	  DWORD		Version;

#define R3D_INIT_BINARY_HEADER(hdr, def)	\
  memset(&hdr, 0, sizeof(hdr));			\
  hdr.r3dID   = R3D_BINARY_FILE_ID;		\
  hdr.ID      = def##_BINARY_ID;		\
  hdr.Version = def##_BINARY_VER;

#define R3D_CHECK_BINARY_HEADER(hdr, def)	\
  (hdr.ID == def##_BINARY_ID && hdr.Version == def##_BINARY_VER)

#endif	// __R3D_BINFMT_H
