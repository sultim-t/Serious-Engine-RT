#ifndef SE_INCL_PATHFINDING_H
#define SE_INCL_PATHFINDING_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

// temporary structure used for path finding
class DECL_DLL CPathNode {
public:
  // cunstruction/destruction
  CPathNode(class CNavigationMarker *penMarker);
  ~CPathNode(void);

  // get name of this node
  const CTString &GetName(void);

  // get link with given index or null if no more (for iteration along the graph)
  CPathNode *GetLink(INDEX i);

  class CNavigationMarker *pn_pnmMarker; // the marker itself

  CListNode pn_lnInOpen;  // for linking in open/closed lists
  CListNode pn_lnInClosed;

  CPathNode *pn_ppnParent;  // best found parent in path yet
  FLOAT pn_fG;  // total cost to get here through the best parent
  FLOAT pn_fH;  // estimate of distance to the goal
  FLOAT pn_fF;  // total quality of the path going through this node
};

// find first marker for path navigation
DECL_DLL void PATH_FindFirstMarker(
    CEntity *penThis, const FLOAT3D &vSrc, const FLOAT3D &vDst, CEntity *&penMarker, FLOAT3D &vPath);
// find next marker for path navigation
DECL_DLL void PATH_FindNextMarker(
    CEntity *penThis, const FLOAT3D &vSrc, const FLOAT3D &vDst, CEntity *&penMarker, FLOAT3D &vPath);


#endif  /* include-once check. */

