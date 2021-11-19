
#if 0 // use this part when manually setting weapon positions

  _pShell->DeclareSymbol("persistent user FLOAT wpn_fH[30+1];",    &wpn_fH);
  _pShell->DeclareSymbol("persistent user FLOAT wpn_fP[30+1];",    &wpn_fP);
  _pShell->DeclareSymbol("persistent user FLOAT wpn_fB[30+1];",    &wpn_fB);
  _pShell->DeclareSymbol("persistent user FLOAT wpn_fX[30+1];",    &wpn_fX);
  _pShell->DeclareSymbol("persistent user FLOAT wpn_fY[30+1];",    &wpn_fY);
  _pShell->DeclareSymbol("persistent user FLOAT wpn_fZ[30+1];",    &wpn_fZ);
  _pShell->DeclareSymbol("persistent user FLOAT wpn_fFOV[30+1];",  &wpn_fFOV);
  _pShell->DeclareSymbol("persistent user FLOAT wpn_fClip[30+1];", &wpn_fClip);
  _pShell->DeclareSymbol("persistent user FLOAT wpn_fFX[30+1];", &wpn_fFX);
  _pShell->DeclareSymbol("persistent user FLOAT wpn_fFY[30+1];", &wpn_fFY);
//  _pShell->DeclareSymbol("persistent user FLOAT wpn_fFZ[30+1];", &wpn_fFZ);
#else
  _pShell->DeclareSymbol("persistent user FLOAT wpn_fH[30+1];", &wpn_fH);
  _pShell->DeclareSymbol("persistent user FLOAT wpn_fP[30+1];", &wpn_fP);
  _pShell->DeclareSymbol("persistent user FLOAT wpn_fX[30+1];", &wpn_fX);
  _pShell->DeclareSymbol("persistent user FLOAT wpn_fY[30+1];", &wpn_fY);
  _pShell->DeclareSymbol("persistent user FLOAT wpn_fZ[30+1];", &wpn_fZ);

#pragma warning(disable: 4305)

wpn_fH[0]=10;
wpn_fH[1]=-1;
wpn_fH[2]=-5;
wpn_fH[3]=-5;
wpn_fH[4]=0;
wpn_fH[5]=2;
wpn_fH[6]=(FLOAT)3;
wpn_fH[7]=(FLOAT)2;
wpn_fH[8]=(FLOAT)2;
wpn_fH[9]=(FLOAT)0;
wpn_fH[10]=(FLOAT)-12;
wpn_fH[11]=(FLOAT)4;
wpn_fH[12]=(FLOAT)4.6;
wpn_fH[13]=(FLOAT)0;
wpn_fH[14]=(FLOAT)1;
wpn_fH[15]=(FLOAT)2.2;
wpn_fH[16]=(FLOAT)2.5;
wpn_fH[17]=(FLOAT)0;
wpn_fH[18]=(FLOAT)0;
wpn_fH[19]=(FLOAT)0;
wpn_fH[20]=(FLOAT)0;
wpn_fH[21]=(FLOAT)0;
wpn_fH[22]=(FLOAT)0;
wpn_fH[23]=(FLOAT)0;
wpn_fH[24]=(FLOAT)0;
wpn_fH[25]=(FLOAT)0;
wpn_fH[26]=(FLOAT)0;
wpn_fH[27]=(FLOAT)0;
wpn_fH[28]=(FLOAT)0;
wpn_fH[29]=(FLOAT)0;
wpn_fH[30]=(FLOAT)0;
wpn_fP[0]=0;
wpn_fP[1]=2;
wpn_fP[2]=0;
wpn_fP[3]=0;
wpn_fP[4]=2;
wpn_fP[5]=1;
wpn_fP[6]=(FLOAT)2;
wpn_fP[7]=(FLOAT)0;
wpn_fP[8]=(FLOAT)1;
wpn_fP[9]=(FLOAT)10;
wpn_fP[10]=(FLOAT)0;
wpn_fP[11]=(FLOAT)0;
wpn_fP[12]=(FLOAT)2.8;
wpn_fP[13]=(FLOAT)0;
wpn_fP[14]=(FLOAT)3;
wpn_fP[15]=(FLOAT)4.7;
wpn_fP[16]=(FLOAT)6;
wpn_fP[17]=(FLOAT)0;
wpn_fP[18]=(FLOAT)0;
wpn_fP[19]=(FLOAT)0;
wpn_fP[20]=(FLOAT)0;
wpn_fP[21]=(FLOAT)0;
wpn_fP[22]=(FLOAT)0;
wpn_fP[23]=(FLOAT)0;
wpn_fP[24]=(FLOAT)0;
wpn_fP[25]=(FLOAT)0;
wpn_fP[26]=(FLOAT)0;
wpn_fP[27]=(FLOAT)0;
wpn_fP[28]=(FLOAT)0;
wpn_fP[29]=(FLOAT)0;
wpn_fP[30]=(FLOAT)0;
wpn_fB[0]=0;
wpn_fB[1]=6;
wpn_fB[2]=0;
wpn_fB[3]=0;
wpn_fB[4]=0;
wpn_fB[5]=0;
wpn_fB[6]=(FLOAT)0;
wpn_fB[7]=(FLOAT)0;
wpn_fB[8]=(FLOAT)0;
wpn_fB[9]=(FLOAT)0;
wpn_fB[10]=(FLOAT)0;
wpn_fB[11]=(FLOAT)0;
wpn_fB[12]=(FLOAT)0;
wpn_fB[13]=(FLOAT)0;
wpn_fB[14]=(FLOAT)0;
wpn_fB[15]=(FLOAT)0;
wpn_fB[16]=(FLOAT)0;
wpn_fB[17]=(FLOAT)0;
wpn_fB[18]=(FLOAT)0;
wpn_fB[19]=(FLOAT)0;
wpn_fB[20]=(FLOAT)0;
wpn_fB[21]=(FLOAT)0;
wpn_fB[22]=(FLOAT)0;
wpn_fB[23]=(FLOAT)0;
wpn_fB[24]=(FLOAT)0;
wpn_fB[25]=(FLOAT)0;
wpn_fB[26]=(FLOAT)0;
wpn_fB[27]=(FLOAT)0;
wpn_fB[28]=(FLOAT)0;
wpn_fB[29]=(FLOAT)0;
wpn_fB[30]=(FLOAT)0;
wpn_fX[0]=0.08;
wpn_fX[1]=0.23;
wpn_fX[2]=0.17;
wpn_fX[3]=0.17;
wpn_fX[4]=0.1;
wpn_fX[5]=0.13;
wpn_fX[6]=(FLOAT)0.1;
wpn_fX[7]=(FLOAT)0.137;
wpn_fX[8]=(FLOAT)0.17;
wpn_fX[9]=(FLOAT)0.15;
wpn_fX[10]=(FLOAT)0.16;
wpn_fX[11]=(FLOAT)0.204;
wpn_fX[12]=(FLOAT)0.204;
wpn_fX[13]=(FLOAT)0;
wpn_fX[14]=(FLOAT)0.141;
wpn_fX[15]=(FLOAT)0.169;
wpn_fX[16]=(FLOAT)0.19;
wpn_fX[17]=(FLOAT)0;
wpn_fX[18]=(FLOAT)0;
wpn_fX[19]=(FLOAT)0;
wpn_fX[20]=(FLOAT)0;
wpn_fX[21]=(FLOAT)0;
wpn_fX[22]=(FLOAT)0;
wpn_fX[23]=(FLOAT)0;
wpn_fX[24]=(FLOAT)0;
wpn_fX[25]=(FLOAT)0;
wpn_fX[26]=(FLOAT)0;
wpn_fX[27]=(FLOAT)0;
wpn_fX[28]=(FLOAT)0;
wpn_fX[29]=(FLOAT)0;
wpn_fX[30]=(FLOAT)0;
wpn_fY[0]=0;
wpn_fY[1]=-0.23;
wpn_fY[2]=-0.2;
wpn_fY[3]=-0.2;
wpn_fY[4]=-0.22;
wpn_fY[5]=-0.21;
wpn_fY[6]=(FLOAT)-0.205;
wpn_fY[7]=(FLOAT)-0.24;
wpn_fY[8]=(FLOAT)-0.325;
wpn_fY[9]=(FLOAT)-0.45;
wpn_fY[10]=(FLOAT)-0.2;
wpn_fY[11]=(FLOAT)-0.102;
wpn_fY[12]=(FLOAT)-0.306;
wpn_fY[13]=(FLOAT)0;
wpn_fY[14]=(FLOAT)-0.174;
wpn_fY[15]=(FLOAT)-0.232;
wpn_fY[16]=(FLOAT)-0.3;
wpn_fY[17]=(FLOAT)0;
wpn_fY[18]=(FLOAT)0;
wpn_fY[19]=(FLOAT)0;
wpn_fY[20]=(FLOAT)0;
wpn_fY[21]=(FLOAT)0;
wpn_fY[22]=(FLOAT)0;
wpn_fY[23]=(FLOAT)0;
wpn_fY[24]=(FLOAT)0;
wpn_fY[25]=(FLOAT)0;
wpn_fY[26]=(FLOAT)0;
wpn_fY[27]=(FLOAT)0;
wpn_fY[28]=(FLOAT)0;
wpn_fY[29]=(FLOAT)0;
wpn_fY[30]=(FLOAT)0;
wpn_fZ[0]=0;
wpn_fZ[1]=-0.05;
wpn_fZ[2]=0.05;
wpn_fZ[3]=0.05;
wpn_fZ[4]=0.05;
wpn_fZ[5]=-0.25;
//wpn_fZ[6]=(FLOAT)-0.1;
wpn_fZ[7]=(FLOAT)-0.3;
wpn_fZ[8]=(FLOAT)-0.1;
//wpn_fZ[9]=(FLOAT)-0.1;
wpn_fZ[10]=(FLOAT)-0.1;
wpn_fZ[11]=(FLOAT)0;
wpn_fZ[12]=(FLOAT)-0.57;
wpn_fZ[13]=(FLOAT)0;
wpn_fZ[14]=(FLOAT)-0.05;
wpn_fZ[15]=(FLOAT)-0.308;
wpn_fZ[16]=(FLOAT)-0.1;
wpn_fZ[17]=(FLOAT)0;
wpn_fZ[18]=(FLOAT)0;
wpn_fZ[19]=(FLOAT)0;
wpn_fZ[20]=(FLOAT)0;
wpn_fZ[21]=(FLOAT)0;
wpn_fZ[22]=(FLOAT)0;
wpn_fZ[23]=(FLOAT)0;
wpn_fZ[24]=(FLOAT)0;
wpn_fZ[25]=(FLOAT)0;
wpn_fZ[26]=(FLOAT)0;
wpn_fZ[27]=(FLOAT)0;
wpn_fZ[28]=(FLOAT)0;
wpn_fZ[29]=(FLOAT)0;
wpn_fZ[30]=(FLOAT)0;
wpn_fFOV[0]= 90;//2 * 1.18f;
wpn_fFOV[1]= 90;//41.5 * 1.18f;
wpn_fFOV[2]= 90;//57 * 1.18f;
wpn_fFOV[3]= 90;//57 * 1.18f;
wpn_fFOV[4]= 90;//41 * 1.18f;
wpn_fFOV[5]= 90;//52.5 * 1.18f;
wpn_fFOV[6]= 90;//(FLOAT)49 * 1.18f;
wpn_fFOV[7]= 90;//(FLOAT)66.9 * 1.18f;
wpn_fFOV[8]= 90;//(FLOAT)66 * 1.18f;
wpn_fFOV[9]= 90;//(FLOAT)85.5 * 1.18f;
wpn_fFOV[10]= 90;//(FLOAT)72 * 1.18f;
wpn_fFOV[11]= 90;//(FLOAT)80 * 1.18f;
wpn_fFOV[12]= 90;//(FLOAT)50 * 1.18f;
wpn_fFOV[13]= 90;//(FLOAT)80 * 1.18f;
wpn_fFOV[14]= 90;//(FLOAT)70.5 * 1.18f;
wpn_fFOV[15]= 90;//(FLOAT)52.5 * 1.18f;
wpn_fFOV[16]= 90;//(FLOAT)50 * 1.18f;
wpn_fFOV[17]=(FLOAT)0;
wpn_fFOV[18]=(FLOAT)0;
wpn_fFOV[19]=(FLOAT)0;
wpn_fFOV[20]=(FLOAT)0;
wpn_fFOV[21]=(FLOAT)0;
wpn_fFOV[22]=(FLOAT)0;
wpn_fFOV[23]=(FLOAT)0;
wpn_fFOV[24]=(FLOAT)0;
wpn_fFOV[25]=(FLOAT)0;
wpn_fFOV[26]=(FLOAT)0;
wpn_fFOV[27]=(FLOAT)0;
wpn_fFOV[28]=(FLOAT)0;
wpn_fFOV[29]=(FLOAT)0;
wpn_fFOV[30]=(FLOAT)0;
wpn_fClip[0]=0;
wpn_fClip[1]=0.1;
wpn_fClip[2]=0.1;
wpn_fClip[3]=0.1;
wpn_fClip[4]=0.1;
wpn_fClip[5]=0.1;
wpn_fClip[6]=0.1;
wpn_fClip[7]=0.1;
wpn_fClip[8]=0.1;
wpn_fClip[9]=0.1;
wpn_fClip[10]=0.1;
wpn_fClip[11]=0.1;
wpn_fClip[12]=0.1;
wpn_fClip[13]=0.1;
wpn_fClip[14]=0.1;
wpn_fClip[15]=0.1;
wpn_fClip[16]=0.1;
wpn_fClip[17]=0;
wpn_fClip[18]=0;
wpn_fClip[19]=0;
wpn_fClip[20]=0;
wpn_fClip[21]=0;
wpn_fClip[22]=0;
wpn_fClip[23]=0;
wpn_fClip[24]=0;
wpn_fClip[25]=0;
wpn_fClip[26]=0;
wpn_fClip[27]=0;
wpn_fClip[28]=0;
wpn_fClip[29]=0;
wpn_fClip[30]=0;
wpn_fFX[0]=(FLOAT)0;
wpn_fFX[1]=(FLOAT)0;
wpn_fFX[2]=(FLOAT)0;
wpn_fFX[3]=(FLOAT)0;
wpn_fFX[4]=(FLOAT)0;
wpn_fFX[5]=(FLOAT)0;
wpn_fFX[6]=(FLOAT)0;
wpn_fFX[7]=(FLOAT)0;
wpn_fFX[8]=(FLOAT)-0.1;
wpn_fFX[9]=(FLOAT)0;
wpn_fFX[10]=(FLOAT)0;
wpn_fFX[11]=(FLOAT)0;
wpn_fFX[12]=(FLOAT)0;
wpn_fFX[13]=(FLOAT)0;
wpn_fFX[14]=(FLOAT)-0.1;
wpn_fFX[15]=(FLOAT)0;
wpn_fFX[16]=(FLOAT)0.25;
wpn_fFX[17]=(FLOAT)0;
wpn_fFX[18]=(FLOAT)0;
wpn_fFX[19]=(FLOAT)0;
wpn_fFX[20]=(FLOAT)0;
wpn_fFX[21]=(FLOAT)0;
wpn_fFX[22]=(FLOAT)0;
wpn_fFX[23]=(FLOAT)0;
wpn_fFX[24]=(FLOAT)0;
wpn_fFX[25]=(FLOAT)0;
wpn_fFX[26]=(FLOAT)0;
wpn_fFX[27]=(FLOAT)0;
wpn_fFX[28]=(FLOAT)0;
wpn_fFX[29]=(FLOAT)0;
wpn_fFX[30]=(FLOAT)0;
wpn_fFY[0]=(FLOAT)0;
wpn_fFY[1]=(FLOAT)0;
wpn_fFY[2]=(FLOAT)0;
wpn_fFY[3]=(FLOAT)0;
wpn_fFY[4]=(FLOAT)0;
wpn_fFY[5]=(FLOAT)0;
wpn_fFY[6]=(FLOAT)0;
wpn_fFY[7]=(FLOAT)0;
wpn_fFY[8]=(FLOAT)0.11;
wpn_fFY[9]=(FLOAT)0;
wpn_fFY[10]=(FLOAT)0;
wpn_fFY[11]=(FLOAT)0;
wpn_fFY[12]=(FLOAT)0;
wpn_fFY[13]=(FLOAT)0;
wpn_fFY[14]=(FLOAT)-0.4;
wpn_fFY[15]=(FLOAT)0;
wpn_fFY[16]=(FLOAT)-0.5;
wpn_fFY[17]=(FLOAT)0;
wpn_fFY[18]=(FLOAT)0;
wpn_fFY[19]=(FLOAT)0;
wpn_fFY[20]=(FLOAT)0;
wpn_fFY[21]=(FLOAT)0;
wpn_fFY[22]=(FLOAT)0;
wpn_fFY[23]=(FLOAT)0;
wpn_fFY[24]=(FLOAT)0;
wpn_fFY[25]=(FLOAT)0;
wpn_fFY[26]=(FLOAT)0;
wpn_fFY[27]=(FLOAT)0;
wpn_fFY[28]=(FLOAT)0;
wpn_fFY[29]=(FLOAT)0;
wpn_fFY[30]=(FLOAT)0;

// tommygun
wpn_fH[6]=(FLOAT)3;
wpn_fP[6]=(FLOAT)2;
wpn_fB[6]=(FLOAT)0;
wpn_fX[6]=(FLOAT)0.1;
wpn_fY[6]=(FLOAT)-0.205;
wpn_fZ[6]=(FLOAT)-0.1;
wpn_fFOV[6]= 90;//(FLOAT)49 * 1.18f;
wpn_fClip[6]=0.1;
wpn_fFX[6]=(FLOAT)0;
wpn_fFY[6]=(FLOAT)0;

// grenade launcher
wpn_fH[9]=(FLOAT)0;
wpn_fP[9]=(FLOAT)10;
wpn_fB[9]=(FLOAT)0;
wpn_fX[9]=(FLOAT)0.15;
wpn_fY[9]=(FLOAT)-0.45;
wpn_fZ[9]=(FLOAT)-0.1;
wpn_fFOV[9]= 90;//(FLOAT)44.5 * 1.18f;
wpn_fClip[9]=(FLOAT)0.1;
wpn_fFX[9]=(FLOAT)0;
wpn_fFY[9]=(FLOAT)0;

// iron cannon
wpn_fH[16]=(FLOAT)2.5;
wpn_fP[16]=(FLOAT)6;
wpn_fB[16]=(FLOAT)0;
wpn_fX[16]=(FLOAT)0.19;
wpn_fY[16]=(FLOAT)-0.3;
wpn_fZ[16]=(FLOAT)-0.1;
wpn_fFOV[16]= 90;//(FLOAT)57 * 1.18f;
wpn_fClip[16]=(FLOAT)0.1;
wpn_fFX[16]=(FLOAT)0.25;
wpn_fFY[16]=(FLOAT)-0.5;

#pragma warning(default: 4305)

#endif

