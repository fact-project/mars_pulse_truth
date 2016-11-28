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
!   Author(s): Thomas Bretz <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2006
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MRanForest
//
// ParameterContainer for Forest structure
//
// A random forest can be grown by calling GrowForest.
// In advance SetupGrow must be called in order to initialize arrays and
// do some preprocessing.
// GrowForest() provides the training data for a single tree (bootstrap
// aggregate procedure)
//
// Essentially two random elements serve to provide a "random" forest,
// namely bootstrap aggregating (which is done in GrowForest()) and random
// split selection (which is subject to MRanTree::GrowTree())
//
/////////////////////////////////////////////////////////////////////////////
#include "MRanForest.h"

#include <TMath.h>
#include <TRandom.h>

#include "MHMatrix.h"
#include "MRanTree.h"
#include "MData.h"
#include "MDataArray.h"
#include "MParList.h"

#include "MArrayI.h"
#include "MArrayF.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MRanForest);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor.
//
MRanForest::MRanForest(const char *name, const char *title)
    : fClassify(kTRUE), fNumTrees(100), fNumTry(0), fNdSize(1),
    fRanTree(NULL), fRules(NULL), fMatrix(NULL), fUserVal(-1)
{
    fName  = name  ? name  : "MRanForest";
    fTitle = title ? title : "Storage container for Random Forest";

    fForest=new TObjArray();
    fForest->SetOwner(kTRUE);
}

MRanForest::MRanForest(const MRanForest &rf)
{
    // Copy constructor
    fName  = rf.fName;
    fTitle = rf.fTitle;

    fClassify = rf.fClassify;
    fNumTrees = rf.fNumTrees;
    fNumTry   = rf.fNumTry;
    fNdSize   = rf.fNdSize;
    fTreeNo   = rf.fTreeNo;
    fRanTree  = NULL;

    fRules=new MDataArray();
    fRules->Reset();

    MDataArray *newrules=rf.fRules;

    for(Int_t i=0;i<newrules->GetNumEntries();i++)
    {
        MData &data=(*newrules)[i];
        fRules->AddEntry(data.GetRule());
    }

    // trees
    fForest=new TObjArray();
    fForest->SetOwner(kTRUE);

    TObjArray *newforest=rf.fForest;
    for(Int_t i=0;i<newforest->GetEntries();i++)
    {
        MRanTree *rantree=(MRanTree*)newforest->At(i);

        MRanTree *newtree=new MRanTree(*rantree);
        fForest->Add(newtree);
    }

    fHadTrue  = rf.fHadTrue;
    fHadEst   = rf.fHadEst;
    fDataSort = rf.fDataSort;
    fDataRang = rf.fDataRang;
    fClassPop = rf.fClassPop;
    fWeight   = rf.fWeight;
    fTreeHad  = rf.fTreeHad;

    fNTimesOutBag = rf.fNTimesOutBag;
}

// --------------------------------------------------------------------------
// Destructor. 
MRanForest::~MRanForest()
{
    delete fForest;
    if (fMatrix)
        delete fMatrix;
    if (fRules)
        delete fRules;
}

void MRanForest::Print(Option_t *o) const
{
    *fLog << inf << GetDescriptor() << ": " << endl;
    MRanTree *t = GetTree(0);
    if (t)
    {
        *fLog << "Setting up RF for training on target:" << endl;
        *fLog << " " << t->GetTitle() << endl;
    }
    if (fRules)
    {
        *fLog << "Following rules are used as input to RF:" << endl;
        for (Int_t i=0;i<fRules->GetNumEntries();i++)
            *fLog << " " << i << ") " << (*fRules)[i].GetRule() << endl;
    }
    *fLog << "Random forest parameters:" << endl;
    if (t)
    {
        *fLog << " - " << (t->IsClassify()?"classification":"regression") << " tree" << endl;
        *fLog << " - Number of trys: " << t->GetNumTry() << endl;
        *fLog << " - Node size: " << t->GetNdSize() << endl;
    }
    *fLog << " - Number of trees: " << fNumTrees << endl;
    *fLog << " - User value: " << fUserVal << endl;
    *fLog << endl;
}

void MRanForest::SetNumTrees(Int_t n)
{
    //at least 1 tree
    fNumTrees=TMath::Max(n,1);
}

void MRanForest::SetNumTry(Int_t n)
{
    fNumTry=TMath::Max(n,0);
}

void MRanForest::SetNdSize(Int_t n)
{
    fNdSize=TMath::Max(n,1);
}

void MRanForest::SetWeights(const TArrayF &weights)
{
    fWeight=weights;
}

void MRanForest::SetGrid(const TArrayD &grid)
{
    const int n=grid.GetSize();

    for(int i=0;i<n-1;i++)
        if(grid[i]>=grid[i+1])
        {
            *fLog<<warn<<"Grid points must be in increasing order! Ignoring grid."<<endl;
            return;
        }

    fGrid=grid;

    //*fLog<<inf<<"Following "<<n<<" grid points are used:"<<endl;
    //for(int i=0;i<n;i++)
    //    *fLog<<inf<<" "<<i<<") "<<fGrid[i]<<endl;
}

MRanTree *MRanForest::GetTree(Int_t i) const
{
    return static_cast<MRanTree*>(fForest->UncheckedAt(i));
}

Int_t MRanForest::GetNumDim() const
{
    return fMatrix ? fMatrix->GetNcols() : 0;
}

Int_t MRanForest::GetNumData() const
{
    return fMatrix ? fMatrix->GetNrows() : 0;
}

Int_t MRanForest::GetNclass() const
{
    int maxidx = TMath::LocMax(fClass.GetSize(),fClass.GetArray());

    return int(fClass[maxidx])+1;
}

void MRanForest::PrepareClasses()
{
    const int numdata=fHadTrue.GetSize();

    if(fGrid.GetSize()>0)
    {
        // classes given by grid
        const int ngrid=fGrid.GetSize();

        for(int j=0;j<numdata;j++)
        {
            // Array is supposed  to be sorted prior to this call.
            // If match is found, function returns position of element.
            // If no match found, function gives nearest element smaller
            // than value.
            int k=TMath::BinarySearch(ngrid, fGrid.GetArray(), (Double_t)fHadTrue[j]);

            fClass[j]   = k;
        }

        int minidx = TMath::LocMin(fClass.GetSize(),fClass.GetArray());
        int min = fClass[minidx];
        if(min!=0) for(int j=0;j<numdata;j++)fClass[j]-=min;

    }else{
        // classes directly given
        for (Int_t j=0;j<numdata;j++)
            fClass[j] = TMath::Nint(fHadTrue[j]);
    }
}

Double_t MRanForest::CalcHadroness()
{
    TVector event;
    *fRules >> event;

    return CalcHadroness(event);
}

Double_t MRanForest::CalcHadroness(const TVector &event)
{
    fTreeHad.Set(fNumTrees);

    Double_t hadroness=0;
    Int_t    ntree    =0;

    TIter Next(fForest);

    MRanTree *tree;
    while ((tree=(MRanTree*)Next()))
        hadroness += (fTreeHad[ntree++]=tree->TreeHad(event));

    return hadroness/ntree;
}

Bool_t MRanForest::AddTree(MRanTree *rantree=NULL)
{
    fRanTree = rantree ? rantree : fRanTree;

    if (!fRanTree) return kFALSE;

    MRanTree *newtree=new MRanTree(*fRanTree);
    fForest->Add(newtree);

    return kTRUE;
}

Bool_t MRanForest::SetupGrow(MHMatrix *mat,MParList *plist)
{
    //-------------------------------------------------------------------
    // access matrix, copy last column (target) preliminarily
    // into fHadTrue
    if (fMatrix)
        delete fMatrix;
    fMatrix = new TMatrix(mat->GetM());

    int dim     = fMatrix->GetNcols()-1;
    int numdata = fMatrix->GetNrows();

    fHadTrue.Set(numdata);
    fHadTrue.Reset();

    for (Int_t j=0;j<numdata;j++)
        fHadTrue[j] = (*fMatrix)(j,dim);

    // remove last col
    fMatrix->ResizeTo(numdata,dim);

    //-------------------------------------------------------------------
    // setup labels for classification/regression
    fClass.Set(numdata);
    fClass.Reset();

    if (fClassify)
        PrepareClasses();

    //-------------------------------------------------------------------
    // allocating and initializing arrays
    fHadEst.Set(numdata);
    fHadEst.Reset();

    fNTimesOutBag.Set(numdata);
    fNTimesOutBag.Reset();

    fDataSort.Set(dim*numdata);
    fDataSort.Reset();

    fDataRang.Set(dim*numdata);
    fDataRang.Reset();

    Bool_t useweights = fWeight.GetSize()==numdata;
    if (!useweights)
    {
        fWeight.Set(numdata);
        fWeight.Reset(1.);
        *fLog << inf <<"Setting weights to 1 (no weighting)"<< endl;
    }

    //-------------------------------------------------------------------
    // setup rules to be used for classification/regression
    const MDataArray *allrules=(MDataArray*)mat->GetColumns();
    if (allrules==NULL)
    {
        *fLog << err <<"Rules of matrix == null, exiting"<< endl;
        return kFALSE;
    }

    if (allrules->GetNumEntries()!=dim+1)
    {
        *fLog << err <<"Rules of matrix " << allrules->GetNumEntries() << " mismatch dimension+1 " << dim+1 << "...exiting."<< endl;
        return kFALSE;
    }

    if (fRules)
        delete fRules;

    fRules = new MDataArray();
    fRules->Reset();

    const TString target_rule = (*allrules)[dim].GetRule();
    for (Int_t i=0;i<dim;i++)
        fRules->AddEntry((*allrules)[i].GetRule());

    *fLog << inf << endl;
    *fLog << "Setting up RF for training on target:" << endl;
    *fLog << " " << target_rule.Data() << endl;
    *fLog << "Following rules are used as input to RF:" << endl;
    for (Int_t i=0;i<dim;i++)
        *fLog << " " << i << ") " << (*fRules)[i].GetRule() << endl;
    *fLog << endl;

    //-------------------------------------------------------------------
    // prepare (sort) data for fast optimization algorithm
    if (!CreateDataSort())
        return kFALSE;

    //-------------------------------------------------------------------
    // access and init tree container
    fRanTree = (MRanTree*)plist->FindCreateObj("MRanTree");
    if(!fRanTree)
    {
        *fLog << err << dbginf << "MRanForest, fRanTree not initialized... aborting." << endl;
        return kFALSE;
    }
    //fRanTree->SetName(target_rule); // Is not stored anyhow

    const Int_t tryest = TMath::Nint(TMath::Sqrt(dim));

    *fLog << inf << endl;
    *fLog << "Following input for the tree growing are used:"<<endl;
    *fLog << " Forest type     : "<<(fClassify?"classification":"regression")<<endl;
    *fLog << " Number of Trees : "<<fNumTrees<<endl;
    *fLog << " Number of Trials: "<<(fNumTry==0?tryest:fNumTry)<<(fNumTry==0?" (auto)":"")<<endl;
    *fLog << " Final Node size : "<<fNdSize<<endl;
    *fLog << " Using Grid      : "<<(fGrid.GetSize()>0?"Yes":"No")<<endl;
    *fLog << " Using Weights   : "<<(useweights?"Yes":"No")<<endl;
    *fLog << " Number of Events: "<<numdata<<endl;
    *fLog << " Number of Params: "<<dim<<endl;

    if(fNumTry==0)
    {
        fNumTry=tryest;
        *fLog << inf << endl;
        *fLog << "Set no. of trials to the recommended value of round(";
        *fLog << TMath::Sqrt(dim) << ") = " << fNumTry << endl;
    }

    fRanTree->SetNumTry(fNumTry);
    fRanTree->SetClassify(fClassify);
    fRanTree->SetNdSize(fNdSize);

    fTreeNo=0;

    return kTRUE;
}

Bool_t MRanForest::GrowForest()
{
    if(!gRandom)
    {
        *fLog << err << dbginf << "gRandom not initialized... aborting." << endl;
        return kFALSE;
    }

    fTreeNo++;

    //-------------------------------------------------------------------
    // initialize running output

    float minfloat=TMath::MinElement(fHadTrue.GetSize(),fHadTrue.GetArray());
    Bool_t calcResolution=(minfloat>FLT_MIN);

    if (fTreeNo==1)
    {
        *fLog << inf << endl << underline;

        if(calcResolution)
            *fLog << "TreeNum BagSize NumNodes TestSize  Bias/%   var/%   res/% (from oob-data)" << endl;
        else
            *fLog << "TreeNum BagSize NumNodes TestSize  Bias/au  var/au  rms/au (from oob-data)" << endl;
                     //        12345678901234567890123456789012345678901234567890
    }

    const Int_t numdata = GetNumData();
    const Int_t nclass  = GetNclass();

    //-------------------------------------------------------------------
    // bootstrap aggregating (bagging) -> sampling with replacement:

    MArrayF classpopw(nclass);
    MArrayI jinbag(numdata); // Initialization includes filling with 0
    MArrayF winbag(numdata); // Initialization includes filling with 0

    float square=0;
    float mean=0;

    for (Int_t n=0; n<numdata; n++)
    {
        // The integer k is randomly (uniformly) chosen from the set
        // {0,1,...,numdata-1}, which is the set of the index numbers of
        // all events in the training sample
  
        const Int_t k = gRandom->Integer(numdata);

        if(fClassify)
            classpopw[fClass[k]]+=fWeight[k];
        else
            classpopw[0]+=fWeight[k];

        mean  +=fHadTrue[k]*fWeight[k];
        square+=fHadTrue[k]*fHadTrue[k]*fWeight[k];

        winbag[k]+=fWeight[k]; // Increase weight if chosen more than once
        jinbag[k]=1;
    }

    //-------------------------------------------------------------------
    // modifying sorted-data array for in-bag data:

    // In bagging procedure ca. 2/3 of all elements in the original
    // training sample are used to build the in-bag data
    const MArrayF hadtrue(fHadTrue.GetSize(), fHadTrue.GetArray());
    const MArrayI fclass(fClass.GetSize(), fClass.GetArray());
    const MArrayI datarang(fDataRang.GetSize(), fDataRang.GetArray());

    MArrayI datsortinbag(fDataSort.GetSize(), fDataSort.GetArray());

    ModifyDataSort(datsortinbag, jinbag);

    fRanTree->GrowTree(fMatrix,hadtrue,fclass,datsortinbag,datarang,classpopw,mean,square,
                       jinbag,winbag,nclass);

    const Double_t ferr = EstimateError(jinbag, calcResolution);

    fRanTree->SetError(ferr);

    // adding tree to forest
    AddTree();

    return fTreeNo<fNumTrees;
}

//-------------------------------------------------------------------
// error-estimates from out-of-bag data (oob data):
//
// For a single tree the events not(!) contained in the bootstrap
// sample of this tree can be used to obtain estimates for the
// classification error of this tree.
// If you take a certain event, it is contained in the oob-data of
// 1/3 of the trees (see comment to ModifyData). This means that the
// classification error determined from oob-data is underestimated,
// but can still be taken as upper limit.
//
Double_t MRanForest::EstimateError(const MArrayI &jinbag, Bool_t calcResolution)
{
    const Int_t numdata = GetNumData();

    Int_t ninbag = 0;
    for (Int_t ievt=0;ievt<numdata;ievt++)
    {
        if (jinbag[ievt]>0)
        {
            ninbag++;
            continue;
        }

        fHadEst[ievt] +=fRanTree->TreeHad((*fMatrix), ievt);
        fNTimesOutBag[ievt]++;
    }

    Int_t n=0;

    Double_t sum=0;
    Double_t sq =0;
    for (Int_t i=0; i<numdata; i++)
    {
        if (fNTimesOutBag[i]==0)
            continue;

        const Float_t hadest = fHadEst[i]/fNTimesOutBag[i];

        const Float_t val = calcResolution ?
            hadest/fHadTrue[i] - 1 : hadest - fHadTrue[i];

        sum += val;
        sq  += val*val;
        n++;
    }

    if (calcResolution)
    {
        sum *= 100;
        sq  *= 10000;
    }

    sum /= n;
    sq  /= n;

    const Double_t var  = TMath::Sqrt(sq-sum*sum);
    const Double_t ferr = TMath::Sqrt(sq);

    //-------------------------------------------------------------------
    // give running output
    *fLog << setw(4) << fTreeNo;
    *fLog << Form(" %8.1f", 100.*ninbag/numdata);
    *fLog << setw(9) << fRanTree->GetNumEndNodes();
    *fLog << Form("  %9.1f", 100.*n/numdata);
    *fLog << Form(" %7.2f", sum);
    *fLog << Form(" %7.2f", var);
    *fLog << Form(" %7.2f", ferr);
    *fLog << endl;

    return ferr;
}

Bool_t MRanForest::CreateDataSort()
{
    // fDataSort(m,n) is the event number in which fMatrix(m,n) occurs.
    // fDataRang(m,n) is the rang of fMatrix(m,n), i.e. if rang = r:
    //   fMatrix(m,n) is the r-th highest value of all fMatrix(m,.).
    //
    // There may be more then 1 event with rang r (due to bagging).

    const Int_t numdata = GetNumData();
    const Int_t dim = GetNumDim();

    TArrayF v(numdata);
    TArrayI isort(numdata);


    for (Int_t mvar=0;mvar<dim;mvar++)
    {

        for(Int_t n=0;n<numdata;n++)
        {
            v[n]=(*fMatrix)(n,mvar);
            //isort[n]=n;

            if (!TMath::Finite(v[n]))
            {
                *fLog << err <<"Event no. "<<n<<", matrix column no. "<<mvar;
                *fLog << err <<" has a non finite value (eg. NaN)."<<endl;
                return kFALSE;
            }
        }

        TMath::Sort(numdata,v.GetArray(),isort.GetArray(),kFALSE);

        // this sorts the v[n] in ascending order. isort[n] is the
        // event number of that v[n], which is the n-th from the
        // lowest (assume the original event numbers are 0,1,...).

        // control sorting
        /*
        for(int n=0;n<numdata-1;n++)
            if(v[isort[n]]>v[isort[n+1]])
            {
                *fLog << err <<"Event no. "<<n<<", matrix column no. "<<mvar;
                *fLog << err <<" not at correct sorting position."<<endl;
                return kFALSE;
            }
            */

        // DataRang is similar to the isort index starting from 0 it is
        // increased by one for each event which is greater, but stays
        // the same for the same value. (So to say it counts how many
        // different values we have)
        for(Int_t n=0;n<numdata-1;n++)
        {
            const Int_t n1=isort[n];
            const Int_t n2=isort[n+1];

            // FIXME: Copying isort[n] to fDataSort[mvar*numdata]
            // can be accelerated!
            fDataSort[mvar*numdata+n]=n1;
            if(n==0) fDataRang[mvar*numdata+n1]=0;
            if(v[n1]<v[n2])
            {
                fDataRang[mvar*numdata+n2]=fDataRang[mvar*numdata+n1]+1;
            }else{
                fDataRang[mvar*numdata+n2]=fDataRang[mvar*numdata+n1];
            }
        }
        fDataSort[(mvar+1)*numdata-1]=isort[numdata-1];
    }
    return kTRUE;
}

// Reoves all indices which are not in the bag from the datsortinbag
void MRanForest::ModifyDataSort(MArrayI &datsortinbag, const MArrayI &jinbag)
{
    const Int_t numdim=GetNumDim();
    const Int_t numdata=GetNumData();

    Int_t ninbag=0;
    for (Int_t n=0;n<numdata;n++)
        if(jinbag[n]==1) ninbag++;

    for(Int_t m=0;m<numdim;m++)
    {
        Int_t *subsort = &datsortinbag[m*numdata];

        Int_t k=0;
        for(Int_t n=0;n<ninbag;n++)
        {
            if(jinbag[subsort[k]]==1)
            {
                subsort[n] = subsort[k];
                k++;
            }else{
                for(Int_t j=k+1;j<numdata;j++)
                {
                    if(jinbag[subsort[j]]==1)
                    {
                        subsort[n] = subsort[j];
                        k = j+1;
                        break;
                    }
                }
            }
        }
    }
}

Bool_t MRanForest::AsciiWrite(ostream &out) const
{
    Int_t n=0;
    MRanTree *tree;
    TIter forest(fForest);

    while ((tree=(MRanTree*)forest.Next()))
    {
        tree->AsciiWrite(out);
        n++;
    }

    return n==fNumTrees;
}
