#include <iostream>

#include <TSystem.h>

#include "MSQLMagic.h"
#include "MSequenceSQL.h"

using namespace std;

void stage(MSequence &s, TString path)
{
    MDirIter iter1, iter2;

    s.GetRuns(iter1, MSequence::kRawPed, path);
    s.GetRuns(iter1, MSequence::kRawCal, path);
    s.GetRuns(iter1, MSequence::kRawDat, path);

    TString req = "ssh -nx phoenix /opt/SUNWsamfs/bin/stage -V";
    while (1)
    {
        TString name = iter1.Next();
        if (name.IsNull())
            break;

        req += " "+name;
    }

    gSystem->Exec(req);
}

int stage(Int_t sequno, Int_t tel, TString path="")
{
    const MSequenceSQL s("sql.rc", sequno, tel);
    if (!s.IsValid())
        return 2;

    stage(s, path);

    return 0;
}

int stage(const char *seq, TString path="")
{
    const MSequence s(seq);
    if (!s.IsValid())
        return 2;

    stage(s, path);

    return 0;
}
