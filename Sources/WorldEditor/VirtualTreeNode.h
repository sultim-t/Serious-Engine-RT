/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

// VirtualTreeNode.h : main header file for the Virtual Tree Class
//
#ifndef VIRTUALTREENODE_H
#define VIRTUALTREENODE_H 1

enum BrowsingMode {
  BM_ICONS_SMALL = 0,
  BM_ICONS_MEDIUM,
  BM_ICONS_LARGE,
  BM_DESCRIPTION,
  BM_FILENAME,
  BM_ICONS_MICRO,
};

#define NO_OF_ICONS 16

class CDropVirtualTreeNode {
public:
  char dvtn_chrDragName[256];
};

class CVirtualTreeNode {
public:
  BOOL vtn_bIsDirectory;              // set if this is directory
  CListNode vtn_lnInDirectory;
  CListHead vtn_lhChildren;           // valid if this is a directory
  CVirtualTreeNode *vnt_pvtnParent;   // NULL if this is root
  ULONG vtn_Handle;                   // internaly use ULONG, later cast to HTREEITEM
  INDEX vtn_itIconType;               // INDEX of representing icon in list of icons
  enum BrowsingMode vtn_bmBrowsingMode;// how to display items - as icons, text, ...
  BOOL vtn_bSelected;                 // (if item) is this item curently selected
  CTextureData *vtn_pTextureData;     // here we will load textures or thumb nails 
  CTString vtn_strName;               // (if directory) name in virtual tree
  CTFileName vtn_fnItem;              // (if item) file name

  // Functions
	CVirtualTreeNode();			            // constructor and
	~CVirtualTreeNode();			          // destructor 
  void MakeRoot(void);
  void Dump(CTStream *pFile);
	void Read_t( CTStream *pFile, CVirtualTreeNode* pParent);	// read function
	void Write_t( CTStream *pFile);	    	// write function
  void MoveToDirectory(CVirtualTreeNode *pVTNDst);
};

#endif // VIRTUALTREENODE_H
