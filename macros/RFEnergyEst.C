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
!   Author(s): Thomas Hengstebeck, 02/2005 <mailto:hengsteb@physik.hu-berlin.de>
!
!   Copyright: MAGIC Software Development, 2000-2005
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
// This macro shows you how to use the new class MRFEnergyEst, which is
// an implementation of energy estimation with RF. It contains following
// functions:
//
//   - CreateMatrices()  : Create matrix for training (and matrix for test)
//   - RFEnergyEstTrain(): Training of RFs
//   - ReadForests()     : Read and print out energy bins which have been used
//                         in training (depence on statistics)
//
//   - RFEnergyEstTest() : Fast testing using a matrix with test data
//   - RFEnergyEstLoop() : Application of RF energy est in eventloop
//
////////////////////////////////////////////////////////////////////////////


//****************************************************************************
// main user settings
//****************************************************************************

//----------------------------------------------------------------------------
// settings for RF training
const Int_t ntrees    = 50; // 50-100 is usually sufficient
const Int_t numtrials = 3;  // should be <= sqrt(no. of used var)
const Int_t nodesize  = 1;  // best results with 1

// settings for energy grid
const Int_t ebins   = 30;
const Float_t e_low = log10(10);    // lower limit of log10(energy[GeV])
const Float_t e_up  = log10(30000); // upper limit

//----------------------------------------------------------------------------
// data settings
TString path="/emc/commich/Mars/mcdata/oldout/";

//TString nameTrain = "19990101_10001_I_MCGamTrainLZA_E_10_5";
//TString nameTest  = "19990101_10002_I_MCGamTestLZA_E_10_5";
TString nameTrain = "19990101_10003_I_MCGamTrainHZA_E_10_5";
TString nameTest  = "19990101_10004_I_MCGamTestHZA_E_10_5" ;

TString info=""; // put here additional info e.g. about cuts used in
                 // CreateMatrices function

//----------------------------------------------------------------------------
// generate filenames
TString fileNameTrain = path + nameTrain + ".root";
TString fileNameTest  = path + nameTest  + ".root";

TString fileMatrixTrain = nameTrain + "_Matrix" + info + ".root";
TString fileMatrixTest  = nameTest  + "_Matrix" + info + ".root";

TString fileForest = "EForests" + nameTrain + info + ".root";


//****************************************************************************
//   Create matrices for training (and test)
//****************************************************************************
void CreateMatrices()
{
    MGeomCamMagic gcam;
    const Double_t mm2deg=gcam.GetConvMm2Deg();//180./17000./3.14159265358979312;

    TString filename[2]   = {fileNameTrain,fileNameTest};
    TString filematrix[2] = {fileMatrixTrain,fileMatrixTest};

    for(int i=0;i<2;i++)
    {
        if(filename[i].IsNull() || filematrix[i].IsNull())
            continue;

        MParList  plist;
        MTaskList tlist;
        plist.AddToList(&tlist);

        MReadTree  read("Events", filename[i]);
        read.DisableAutoScheme();

        MHMatrix matrix("MatrixGammas");
        // setting rules (MC energy must be last column!!!)
        matrix.AddColumn("log10(MHillas.fSize)");
        matrix.AddColumn("MHillasSrc.fDist");
        matrix.AddColumn("MHillas.fWidth");
        matrix.AddColumn("MHillas.fLength");
        matrix.AddColumn("log10(MHillas.fSize/(MHillas.fLength*MHillas.fWidth))");
        matrix.AddColumn("MNewImagePar.fConc");
        matrix.AddColumn("MNewImagePar.fLeakage1");
        matrix.AddColumn("MPointingPos.fZd");
        matrix.AddColumn("MMcEvt.fEnergy");

        plist.AddToList(&matrix);
        MFillH fillmat("MatrixGammas");

        // pre-cuts on data,
        // take care that this is inverted logic (MContinue task!!)
        MContinue sizecut("MHillas.fSize<60.");
        MContinue leakcut("MNewImagePar.fLeakage1>0.1");
        MContinue distcutlo(Form("MHillasSrc.fDist*%f<0.3",mm2deg));
        MContinue distcutup(Form("MHillasSrc.fDist*%f>1.1",mm2deg));
        MContinue hcut("MHadronness.fHadronness>0.3");

        tlist.AddToList(&read);

        // put cuts into tlist
        tlist.AddToList(&sizecut);
        tlist.AddToList(&leakcut);
        tlist.AddToList(&distcutlo);
        tlist.AddToList(&distcutup);
        //tlist.AddToList(&hcut);

        tlist.AddToList(&fillmat);

        MEvtLoop evtloop;
        evtloop.SetParList(&plist);

        if (!evtloop.Eventloop()) return;
        tlist.PrintStatistics();

        TFile file(filematrix[i],"recreate","");
        matrix.Write();
        file.Close();
    }

    return;
}

//****************************************************************************
//   Training of RFs
//****************************************************************************
void RFEnergyEstTrain()
{
    // initializations
    TArrayD egrid(ebins+1);

    for(Int_t i=0;i<=ebins;i++)
        egrid[i]=e_low+i*(e_up-e_low)/float(ebins);

    MHMatrix matrix;
    TFile *file=new TFile(fileMatrixTrain);
    matrix.Read("MatrixGammas");

    // output info about used rules
    cout<<endl<<"Rules for energy estimation:"<<endl;
    for(Int_t i=0;i<matrix.GetM().GetNcols();i++)
    {
        MDataArray *rules = matrix.GetColumns();
        MData &data=(*rules)[i];

        cout<<" "<<i+1<<") "<<data.GetRule()<<endl;
    }

    // setup RF for energy estimation
    MRFEnergyEst rf;
    rf.SetMatrixTrain(&matrix);
    rf.SetFile(fileForest);
    rf.SetLogEnergyGrid(egrid);

    rf.SetNumTrees(ntrees);  // number of trees
    rf.SetNumTry(numtrials); // number of trials in random split selection
    rf.SetNdSize(nodesize);  // limit for nodesize

    rf.Train();

    return;
}

//****************************************************************************
//   Check which energy bins have been used
//****************************************************************************
void ReadForests()
{
    TFile fileRF(fileForest,"read");
    TList *list=(TList*)fileRF.GetListOfKeys();
    const Int_t n=list->GetSize()-1;// subtract 1 since 1 key belongs to MDataArray

    MRanForest forest;
    for(Int_t i=0;i<n;i++)
    {
        forest.Read(Form("%d",i));
        MRanForest *curforest=(MRanForest*)forest.Clone();
        const char *energy=list->FindObject(Form("%d",i))->GetTitle();
        const char *bin   =list->FindObject(Form("%d",i))->GetName();

        if(i<10) cout<<"Bin  "<<bin<<":   log10(Energy[GeV]) = "<<energy<<endl;
        else     cout<<"Bin "<< bin<<":   log10(Energy[GeV]) = "<<energy<<endl;
    }
    fileRF.Close();

    return;
}

//****************************************************************************
//   Fast Testing with matrix
//****************************************************************************
void RFEnergyEstTest()
{
    MHMatrix matrix;
    TFile *file=new TFile(fileMatrixTest);
    matrix.Read("MatrixGammas");

    MRFEnergyEst rf;
    rf.SetMatrixTest(&matrix);
    rf.SetFile(fileForest);

    rf.SetBit(MParContainer::kEnableGraphicalOutput,1);
    rf.Test();

    return;
}

//****************************************************************************
//   Apply RF energy estimation in eventloop
//****************************************************************************
void RFEnergyEstLoop()
{
    MParList plist;

    MTaskList tlist;
    plist.AddToList(&tlist);
    MReadTree  read("Events",fileNameTest);
    read.DisableAutoScheme();

    MRFEnergyEst rf;
    rf.SetFile(fileForest);

    MWriteRootFile write("EnergyEstTest.root");
    write.AddContainer("MMcEvt",     "Events");
    write.AddContainer("MEnergyEst", "Events");

    tlist.AddToList(&read);
    tlist.AddToList(&rf);
    tlist.AddToList(&write);

    MEvtLoop evtloop;
    evtloop.SetParList(&plist);

    if (!evtloop.Eventloop()) return;
    tlist.PrintStatistics();

    return;
}
