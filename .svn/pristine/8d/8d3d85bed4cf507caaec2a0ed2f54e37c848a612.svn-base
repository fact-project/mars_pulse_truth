Bool_t check(const char *path)
{
    cout << "Compiling " << path << "...";

    if (!gSystem->CompileMacro(Form("datacenter/macros/%s", path), "cf"))
        return kFALSE;

    cout << "done." << endl;

    return kTRUE;
}

int checkaclic()
{
    gLog.SetDebugLevel(2);

    if (!check("buildsequenceentries.C"))
        return 1;
    if (!check("checkfileavail.C"))
        return 1;
    if (!check("checkstardone.C"))
        return 1;
    if (!check("doexclusions.C"))
        return 1;
    if (!check("resetcolumn.C"))
        return 1;
    if (!check("plotstat.C"))
        return 1;
    if (!check("plotdb.C"))
        return 1;
    if (!check("fillstar.C"))
        return 1;
    if (!check("fillsinope.C"))
        return 1;
    if (!check("fillsignal.C"))
        return 1;
    if (!check("fillganymed.C"))
        return 1;
    if (!check("filldotrun.C"))
        return 1;
    if (!check("filldotrbk.C"))
        return 1;
    if (!check("filldotraw.C"))
        return 1;
    if (!check("fillcalib.C"))
        return 1;
    if (!check("findcacofiles.C"))
        return 1;
    if (!check("insertcacofile.C"))
        return 1;
    if (!check("insertdataset.C"))
        return 1;
    if (!check("insertdate.C"))
        return 1;
    if (!check("resetallruns.C"))
        return 1;
    if (!check("writesequencefile.C"))
        return 1;

    return 0;
}
