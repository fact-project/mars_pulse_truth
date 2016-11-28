/* ======================================================================== *\
!
! *
! * This file is part of MARS, the MAGIC Analysis and Reconstruction
! * Software. It is distributed to you in the hope that it can be a useful
! * and timesaving tool in analysing Data of imaging Cerenkov telescopes.
! * It is distributed WITHOUT ANY WARRANTY.
! *
! * Permission to use, copy, modify and distribute this software and its
! * documentation for any purpose is hereby granted without fee,
! * provided that the above copyright notice appear in all copies and
! * that both that copyright notice and this permission notice appear
! * in supporting documentation. It is provided "as is" without express
! * or implied warranty.
! *
!
!
!   Author(s): Thomas Hengstebeck 3/2003 <mailto:hengsteb@physik.hu-berlin.de>
!
!   Copyright: MAGIC Software Development, 2000-2005
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MRanTree
//
// ParameterContainer for Tree structure
//
/////////////////////////////////////////////////////////////////////////////
#include "MRanTree.h"

#include <iostream>

#include <TRandom.h>

#include "MArrayI.h"
#include "MArrayF.h"

#include "MMath.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MRanTree);

using namespace std;


// --------------------------------------------------------------------------
// Default constructor.
//
MRanTree::MRanTree(const char *name, const char *title):fClassify(kTRUE),fNdSize(0), fNumTry(3)
{

    fName  = name  ? name  : "MRanTree";
    fTitle = title ? title : "Storage container for structure of a single tree";
}

// --------------------------------------------------------------------------
// Copy constructor
//
MRanTree::MRanTree(const MRanTree &tree)
{
    fName  = tree.fName;
    fTitle = tree.fTitle;

    fClassify = tree.fClassify;
    fNdSize   = tree.fNdSize;
    fNumTry   = tree.fNumTry;

    fNumNodes    = tree.fNumNodes;
    fNumEndNodes = tree.fNumEndNodes;

    fBestVar   = tree.fBestVar;
    fTreeMap1  = tree.fTreeMap1;
    fTreeMap2  = tree.fTreeMap2;
    fBestSplit = tree.fBestSplit;
    fGiniDec   = tree.fGiniDec;
}

void MRanTree::SetNdSize(Int_t n)
{
    // threshold nodesize of terminal nodes, i.e. the training data is
    // splitted until there is only pure date in the subsets(=terminal
    // nodes) or the subset size is LE n

    fNdSize=TMath::Max(1,n);//at least 1 event per node
}

void MRanTree::SetNumTry(Int_t n)
{
    // number of trials in random split selection:
    // choose at least 1 variable to split in

    fNumTry=TMath::Max(1,n);
}

void MRanTree::GrowTree(TMatrix *mat, const MArrayF &hadtrue, const MArrayI &idclass,
                        MArrayI &datasort, const MArrayI &datarang, const MArrayF &tclasspop,
                        const Float_t &mean, const Float_t &square, const MArrayI &jinbag, const MArrayF &winbag,
                        const int nclass)
{
    // arrays have to be initialized with generous size, so number of total nodes (nrnodes)
    // is estimated for worst case
    const Int_t numdim =mat->GetNcols();
    const Int_t numdata=winbag.GetSize();
    const Int_t nrnodes=2*numdata+1;

    // number of events in bootstrap sample
    Int_t ninbag=0;
    for (Int_t n=0;n<numdata;n++) if(jinbag[n]==1) ninbag++;

    MArrayI bestsplit(nrnodes);
    MArrayI bestsplitnext(nrnodes);

    fBestVar.Set(nrnodes);    fBestVar.Reset();
    fTreeMap1.Set(nrnodes);   fTreeMap1.Reset();
    fTreeMap2.Set(nrnodes);   fTreeMap2.Reset();
    fBestSplit.Set(nrnodes);  fBestSplit.Reset();
    fGiniDec.Set(numdim);     fGiniDec.Reset();


    if(fClassify)
        FindBestSplit=&MRanTree::FindBestSplitGini;
    else
        FindBestSplit=&MRanTree::FindBestSplitSigma;

    // tree growing
    BuildTree(datasort,datarang,hadtrue,idclass,bestsplit, bestsplitnext,
              tclasspop,mean,square,winbag,ninbag,nclass);

    // post processing, determine cut (or split) values fBestSplit
    for(Int_t k=0; k<nrnodes; k++)
    {
        if (GetNodeStatus(k)==-1)
            continue;

        const Int_t &bsp =bestsplit[k];
        const Int_t &bspn=bestsplitnext[k];
        const Int_t &msp =fBestVar[k];

        fBestSplit[k] = ((*mat)(bsp, msp)+(*mat)(bspn,msp))/2;
    }

    // resizing arrays to save memory
    fBestVar.Set(fNumNodes);
    fTreeMap1.Set(fNumNodes);
    fTreeMap2.Set(fNumNodes);
    fBestSplit.Set(fNumNodes);
}

int MRanTree::FindBestSplitGini(const MArrayI &datasort,const MArrayI &datarang,
                                const MArrayF &hadtrue,const MArrayI &idclass,
                                Int_t ndstart,Int_t ndend, const MArrayF &tclasspop,
                                const Float_t &mean, const Float_t &square, Int_t &msplit,
                                Float_t &decsplit,Int_t &nbest, const MArrayF &winbag,
                                const int nclass)
{
    const Int_t nrnodes = fBestSplit.GetSize();
    const Int_t numdata = (nrnodes-1)/2;
    const Int_t mdim = fGiniDec.GetSize();

    // For the best split, msplit is the index of the variable (e.g
    // Hillas par., zenith angle ,...)
    // split on. decsplit is the decreae in impurity measured by
    // Gini-index. nsplit is the case number of value of msplit split on,
    // and nsplitnext is the case number of the next larger value of msplit.

    Int_t nbestvar=0;

    // compute initial values of numerator and denominator of Gini-index,
    // Gini index= pno/dno
    Double_t pno=0;
    Double_t pdo=0;

    // tclasspop: sum of weights for events in class
    for (Int_t j=0; j<nclass; j++) // loop over number of classes to classifiy
    {
        pno+=tclasspop[j]*tclasspop[j];
        pdo+=tclasspop[j];
    }

    const Double_t crit0=pno/pdo;  // weighted mean of weights

    // start main loop through variables to find best split,
    // (Gini-index as criterium crit)

    Double_t critmax=-FLT_MAX;

    // random split selection, number of trials = fNumTry
    for (Int_t mt=0; mt<fNumTry; mt++) // we could try ALL variables???
    {
        const Int_t mvar= gRandom->Integer(mdim);
        const Int_t mn  = mvar*numdata;

        // Gini index = rrn/rrd+rln/rld
        Double_t rrn=pno;
        Double_t rrd=pdo;
        Double_t rln=0;
        Double_t rld=0;

        MArrayF wl(nclass);     // left node //nclass
        MArrayF wr(tclasspop);  // right node//nclass

        Double_t critvar=-FLT_MAX;
        for(Int_t nsp=ndstart;nsp<=ndend-1;nsp++)
        {
            const Int_t  &nc = datasort[mn+nsp];
            const Int_t   &k = idclass[nc];
            const Float_t &u = winbag[nc];

            // do classification, Gini index as split rule
            rln   +=u*(2*wl[k]+u);  // += u*(wl[k]{i-1} + wl[k]{i-1}+u{i})
            rld   +=u;   // sum of weights left  from cut total
            wl[k] +=u;   // sum of weights left  from cut for class k

            rrn   -=u*(2*wr[k]-u);  // -= u*(wr[k]{i-1} + wr[k]{i-1}-u{i})
            //  rr0=0; rr0+=u*2*tclasspop[k]
            //  rrn = pno - rr0 + rln
            rrd   -=u;   // sum of weights right from cut total
            wr[k] -=u;   // sum of weights right from cut for class k

            // REPLACE BY?
            // rr0   = 0
            // rr0  += u*2*tclasspop[k]
            // rrn   = pno - rr0 + rln
            // rrd   = pdo - rld
            // wr[k] = tclasspop[k] - wl[k]

            // crit = (rln*(pdo - rld + 1) + pno - rr0) / rld*(pdo - rld)

            /*
             if (k==background)
                continue;
             crit = TMath::Max(MMath::SignificanceLiMa(rld, rld-wl[k]),
                               MMath::SignificanceLiMa(rrd, rrd-wr[k]))
             */

            // This condition is in fact a == (> cannot happen at all)
            // This is because we cannot set the cut between two identical values
            //if (datarang[mn+datasort[mn+nsp]]>=datarang[mn+datasort[mn+nsp+1]])
            if (datarang[mn+nc]>=datarang[mn+datasort[mn+nsp+1]])
                continue;

            // If crit starts to become pretty large do WHAT???
            //if (TMath::Min(rrd,rld)<=1.0e-5) // FIXME: CHECKIT FOR WEIGHTS!
            //    continue;

            const Double_t crit=(rln/rld)+(rrn/rrd);
            if (!TMath::Finite(crit))
                continue;

            // Search for the highest value of crit
            if (crit<=critvar) continue;

            // store the highest crit value and the corresponding event to cut at
            nbestvar=nsp;
            critvar=crit;
        }

        if (critvar<=critmax) continue;

        msplit=mvar;      // Variable in which to split
        nbest=nbestvar;   // event at which the best split was found
        critmax=critvar;
    }

    // crit0 = MMath::SignificanceLiMa(pdo, pdo-tclasspop[0])
    // mean increase of sensitivity
    // decsplit = sqrt(critmax/crit0)
    decsplit=critmax-crit0;

    return critmax<-1.0e10 ? 1 : 0;
}

int MRanTree::FindBestSplitSigma(const MArrayI &datasort,const MArrayI &datarang,
                                 const MArrayF &hadtrue, const MArrayI &idclass,
                                 Int_t ndstart,Int_t ndend, const MArrayF &tclasspop,
                                 const Float_t &mean, const Float_t &square, Int_t &msplit,
                                 Float_t &decsplit,Int_t &nbest, const MArrayF &winbag,
                                 const int nclass)
{
    const Int_t nrnodes = fBestSplit.GetSize();
    const Int_t numdata = (nrnodes-1)/2;
    const Int_t mdim = fGiniDec.GetSize();

    // For the best split, msplit is the index of the variable (e.g
    // Hillas par., zenith angle ,...) split on. decsplit is the decreae
    // in impurity measured by Gini-index. nsplit is the case number of
    // value of msplit split on, and nsplitnext is the case number of
    // the next larger value of msplit.

    Int_t nbestvar=0;

    // compute initial values of numerator and denominator of split-index,

    // resolution
    //Double_t pno=-(tclasspop[0]*square-mean*mean)*tclasspop[0];
    //Double_t pdo= (tclasspop[0]-1.)*mean*mean;

    // n*resolution
    //Double_t pno=-(tclasspop[0]*square-mean*mean)*tclasspop[0];
    //Double_t pdo= mean*mean;

    // variance
    //Double_t pno=-(square-mean*mean/tclasspop[0]);
    //Double_t pdo= (tclasspop[0]-1.);

    // n*variance
    Double_t pno= (square-mean*mean/tclasspop[0]);
    Double_t pdo= 1.;

    // 1./(n*variance)
    //Double_t pno= 1.;
    //Double_t pdo= (square-mean*mean/tclasspop[0]);

    const Double_t crit0=pno/pdo;

    // start main loop through variables to find best split,

    Double_t critmin=FLT_MAX;

    // random split selection, number of trials = fNumTry
    for (Int_t mt=0; mt<fNumTry; mt++)
    {
        const Int_t mvar= gRandom->Integer(mdim);
        const Int_t mn  = mvar*numdata;

        Double_t esumr =mean;
        Double_t e2sumr=square;
        Double_t esuml =0;
        Double_t e2suml=0;

        float wl=0.;// left node
        float wr=tclasspop[0]; // right node

        Double_t critvar=critmin;
        for(Int_t nsp=ndstart;nsp<=ndend-1;nsp++)
        {
            const Int_t &nc=datasort[mn+nsp];
            const Float_t &f=hadtrue[nc];;
            const Float_t &u=winbag[nc];

            e2suml+=u*f*f;
            esuml +=u*f;
            wl    +=u;

            //-------------------------------------------
            // resolution
            //const Double_t rln=(wl*e2suml-esuml*esuml)*wl;
            //const Double_t rld=(wl-1.)*esuml*esuml;

            // resolution times n
            //const Double_t rln=(wl*e2suml-esuml*esuml)*wl;
            //const Double_t rld=esuml*esuml;

            // sigma
            //const Double_t rln=(e2suml-esuml*esuml/wl);
            //const Double_t rld=(wl-1.);

            // sigma times n
            Double_t rln=(e2suml-esuml*esuml/wl);
            Double_t rld=1.;

            // 1./(n*variance)
            //const Double_t rln=1.;
            //const Double_t rld=(e2suml-esuml*esuml/wl);
            //-------------------------------------------

            // REPLACE BY??? 
            e2sumr-=u*f*f;   // e2sumr = square       - e2suml
            esumr -=u*f;     // esumr  = mean         - esuml
            wr    -=u;       // wr     = tclasspop[0] - wl

            //-------------------------------------------
            // resolution
            //const Double_t rrn=(wr*e2sumr-esumr*esumr)*wr;
            //const Double_t rrd=(wr-1.)*esumr*esumr;

            // resolution times n
            //const Double_t rrn=(wr*e2sumr-esumr*esumr)*wr;
            //const Double_t rrd=esumr*esumr;

            // sigma
            //const Double_t rrn=(e2sumr-esumr*esumr/wr);
            //const Double_t rrd=(wr-1.);

            // sigma times n
            const Double_t rrn=(e2sumr-esumr*esumr/wr);
            const Double_t rrd=1.;

            // 1./(n*variance)
            //const Double_t rrn=1.;
            //const Double_t rrd=(e2sumr-esumr*esumr/wr);
            //-------------------------------------------

            if (datarang[mn+nc]>=datarang[mn+datasort[mn+nsp+1]])
                continue;

            //if (TMath::Min(rrd,rld)<=1.0e-5)
            //    continue;

            const Double_t crit=(rln/rld)+(rrn/rrd);
            if (!TMath::Finite(crit))
                continue;

            if (crit>=critvar) continue;

            nbestvar=nsp;
            critvar=crit;
        }

        if (critvar>=critmin) continue;

        msplit=mvar;
        nbest=nbestvar;
        critmin=critvar;
    }

    decsplit=crit0-critmin;

    //return critmin>1.0e20 ? 1 : 0;
    return decsplit<0 ? 1 : 0;
}

void MRanTree::MoveData(MArrayI &datasort,Int_t ndstart, Int_t ndend,
                        MArrayI &idmove,MArrayI &ncase,Int_t msplit,
                        Int_t nbest,Int_t &ndendl)
{
    // This is the heart of the BuildTree construction. Based on the
    // best split the data in the part of datasort corresponding to
    // the current node is moved to the left if it belongs to the left
    // child and right if it belongs to the right child-node.
    const Int_t numdata = ncase.GetSize();
    const Int_t mdim    = fGiniDec.GetSize();

    MArrayI tdatasort(numdata);

    // compute idmove = indicator of case nos. going left
    for (Int_t nsp=ndstart;nsp<=ndend;nsp++)
    {
        const Int_t &nc=datasort[msplit*numdata+nsp];
        idmove[nc]= nsp<=nbest?1:0;
    }
    ndendl=nbest;

    // shift case. nos. right and left for numerical variables.
    for(Int_t msh=0;msh<mdim;msh++)
    {
        Int_t k=ndstart-1;
        for (Int_t n=ndstart;n<=ndend;n++)
        {
            const Int_t &ih=datasort[msh*numdata+n];
            if (idmove[ih]==1)
                tdatasort[++k]=datasort[msh*numdata+n];
        }

        for (Int_t n=ndstart;n<=ndend;n++)
        {
            const Int_t &ih=datasort[msh*numdata+n];
            if (idmove[ih]==0)
                tdatasort[++k]=datasort[msh*numdata+n];
        }

        for(Int_t m=ndstart;m<=ndend;m++)
            datasort[msh*numdata+m]=tdatasort[m];
    }

    // compute case nos. for right and left nodes.

    for(Int_t n=ndstart;n<=ndend;n++)
        ncase[n]=datasort[msplit*numdata+n];
}

void MRanTree::BuildTree(MArrayI &datasort,const MArrayI &datarang, const MArrayF &hadtrue,
                         const MArrayI &idclass, MArrayI &bestsplit, MArrayI &bestsplitnext,
                         const MArrayF &tclasspop, const Float_t &tmean, const Float_t &tsquare, const MArrayF &winbag,
                         Int_t ninbag, const int nclass)
{
    // Buildtree consists of repeated calls to two void functions,
    // FindBestSplit and MoveData. Findbestsplit does just that--it finds
    // the best split of the current node. MoveData moves the data in
    // the split node right and left so that the data corresponding to
    // each child node is contiguous.
    //
    // buildtree bookkeeping:
    // ncur is the total number of nodes to date.  nodestatus(k)=1 if
    // the kth node has been split. nodestatus(k)=2 if the node exists
    // but has not yet been split, and =-1 if the node is terminal.
    // A node is terminal if its size is below a threshold value, or
    // if it is all one class, or if all the data-values are equal.
    // If the current node k is split, then its children are numbered
    // ncur+1 (left), and ncur+2(right), ncur increases to ncur+2 and
    // the next node to be split is numbered k+1.  When no more nodes
    // can be split, buildtree returns.
    const Int_t mdim    = fGiniDec.GetSize();
    const Int_t nrnodes = fBestSplit.GetSize();
    const Int_t numdata = (nrnodes-1)/2;

    MArrayI nodepop(nrnodes);
    MArrayI nodestart(nrnodes);
    MArrayI parent(nrnodes);

    MArrayI ncase(numdata);
    MArrayI idmove(numdata);
    MArrayI iv(mdim);

    MArrayF classpop(nrnodes*nclass);//nclass
    MArrayI nodestatus(nrnodes);

    for (Int_t j=0;j<nclass;j++)
        classpop[j*nrnodes+0]=tclasspop[j];

    MArrayF mean(nrnodes);
    MArrayF square(nrnodes);
    MArrayF lclasspop(tclasspop);

    mean[0]=tmean;
    square[0]=tsquare;


    Int_t ncur=0;
    nodepop[0]=ninbag;
    nodestatus[0]=2;

    // start main loop
    for (Int_t kbuild=0; kbuild<nrnodes; kbuild++)
    {
          if (kbuild>ncur) break;
          if (nodestatus[kbuild]!=2) continue;

          // initialize for next call to FindBestSplit

          const Int_t ndstart=nodestart[kbuild];
          const Int_t ndend=ndstart+nodepop[kbuild]-1;

          for (Int_t j=0;j<nclass;j++)
              lclasspop[j]=classpop[j*nrnodes+kbuild];

          Int_t msplit, nbest;
          Float_t decsplit=0;

          if ((this->*FindBestSplit)(datasort,datarang,hadtrue,idclass,ndstart,
                                     ndend, lclasspop,mean[kbuild],square[kbuild],msplit,decsplit,
                                     nbest,winbag,nclass))
          {
              nodestatus[kbuild]=-1;
              continue;
          }

          fBestVar[kbuild]=msplit;
          fGiniDec[msplit]+=decsplit;

          bestsplit[kbuild]=datasort[msplit*numdata+nbest];
          bestsplitnext[kbuild]=datasort[msplit*numdata+nbest+1];

          Int_t ndendl;
          MoveData(datasort,ndstart,ndend,idmove,ncase,
                   msplit,nbest,ndendl);

          // leftnode no.= ncur+1, rightnode no. = ncur+2.
          nodepop[ncur+1]=ndendl-ndstart+1;
          nodepop[ncur+2]=ndend-ndendl;
          nodestart[ncur+1]=ndstart;
          nodestart[ncur+2]=ndendl+1;

          // find class populations in both nodes
          for (Int_t n=ndstart;n<=ndendl;n++)
          {
              const Int_t &nc=ncase[n];
              const int j=idclass[nc];
                   
              // statistics left from cut
              mean[ncur+1]+=hadtrue[nc]*winbag[nc];
              square[ncur+1]+=hadtrue[nc]*hadtrue[nc]*winbag[nc];

              // sum of weights left from cut
              classpop[j*nrnodes+ncur+1]+=winbag[nc];
          }

          for (Int_t n=ndendl+1;n<=ndend;n++)
          {
              const Int_t &nc=ncase[n];
              const int j=idclass[nc];

              // statistics right from cut
              mean[ncur+2]  +=hadtrue[nc]*winbag[nc];
              square[ncur+2]+=hadtrue[nc]*hadtrue[nc]*winbag[nc];

              // sum of weights right from cut
              classpop[j*nrnodes+ncur+2]+=winbag[nc];
          }

          // check on nodestatus

          nodestatus[ncur+1]=2;
          nodestatus[ncur+2]=2;
          if (nodepop[ncur+1]<=fNdSize) nodestatus[ncur+1]=-1;
          if (nodepop[ncur+2]<=fNdSize) nodestatus[ncur+2]=-1;


          Double_t popt1=0;
          Double_t popt2=0;
          for (Int_t j=0;j<nclass;j++)
          {
              popt1+=classpop[j*nrnodes+ncur+1];
              popt2+=classpop[j*nrnodes+ncur+2];
          }

          if(fClassify)
          {
              // check if only members of one class in node
              for (Int_t j=0;j<nclass;j++)
              {
                  if (classpop[j*nrnodes+ncur+1]==popt1) nodestatus[ncur+1]=-1;
                  if (classpop[j*nrnodes+ncur+2]==popt2) nodestatus[ncur+2]=-1;
              }
          }

          fTreeMap1[kbuild]=ncur+1;
          fTreeMap2[kbuild]=ncur+2;
          parent[ncur+1]=kbuild;
          parent[ncur+2]=kbuild;
          nodestatus[kbuild]=1;
          ncur+=2;
          if (ncur>=nrnodes) break;
    }

    // determine number of nodes
    fNumNodes=nrnodes;
    for (Int_t k=nrnodes-1;k>=0;k--)
    {
        if (nodestatus[k]==0) fNumNodes-=1;
        if (nodestatus[k]==2) nodestatus[k]=-1;
    }

    fNumEndNodes=0;
    for (Int_t kn=0;kn<fNumNodes;kn++)
        if(nodestatus[kn]==-1)
        {
            fNumEndNodes++;

            Double_t pp=0;
            for (Int_t j=0;j<nclass;j++)
            {
                if(classpop[j*nrnodes+kn]>pp)
                {
                    // class + status of node kn coded into fBestVar[kn]
                    fBestVar[kn]=j-nclass;
                    pp=classpop[j*nrnodes+kn];
                }
            }

                float sum=0;
                for(int i=0;i<nclass;i++) sum+=classpop[i*nrnodes+kn];

                fBestSplit[kn]=mean[kn]/sum;
        }
}

Double_t MRanTree::TreeHad(const Float_t *evt)
{
    // to optimize on storage space node status and node class
    // are coded into fBestVar:
    // status of node kt = TMath::Sign(1,fBestVar[kt])
    // class  of node kt = fBestVar[kt]+2 (class defined by larger
    //  node population, actually not used)
    // hadronness assigned to node kt = fBestSplit[kt]

    // To get rid of the range check of the root classes
    const Float_t *split = fBestSplit.GetArray();
    const Int_t   *map1  = fTreeMap1.GetArray();
    const Int_t   *map2  = fTreeMap2.GetArray();
    const Int_t   *best  = fBestVar.GetArray();

    Int_t kt=0;
    for (Int_t k=0; k<fNumNodes; k++)
    {
        if (best[kt]<0)
            break;

        const Int_t m=best[kt];
        kt = evt[m]<=split[kt] ? map1[kt] : map2[kt];
    }

    return split[kt];
}

Double_t MRanTree::TreeHad(const TVector &event)
{
    return TreeHad(event.GetMatrixArray());
}

Double_t MRanTree::TreeHad(const TMatrixFRow_const &event)
{
    return TreeHad(event.GetPtr());
}

Double_t MRanTree::TreeHad(const TMatrix &m, Int_t ievt)
{
#if ROOT_VERSION_CODE < ROOT_VERSION(4,00,8)
    return TreeHad(TMatrixRow(m, ievt));
#else
    return TreeHad(TMatrixFRow_const(m, ievt));
#endif
}

Bool_t MRanTree::AsciiWrite(ostream &out) const
{
    TString str;
    Int_t k;

    out.width(5);out<<fNumNodes<<endl;

    for (k=0;k<fNumNodes;k++)
    {
        str=Form("%f",GetBestSplit(k));

        out.width(5);  out << k;
        out.width(5);  out << GetNodeStatus(k);
        out.width(5);  out << GetTreeMap1(k);
        out.width(5);  out << GetTreeMap2(k);
        out.width(5);  out << GetBestVar(k);
        out.width(15); out << str<<endl;
        out.width(5);  out << GetNodeClass(k);
    }
    out<<endl;

    return k==fNumNodes;
}
