void fact_trigger()
{
    MLut lut;

    // read all trigger patches
    lut.ReadFile("resmc/fact-trigger-all.txt");
    // read only the regular trigger patches
    //lut.ReadFile("resmc/fact-trigger-sum.txt");

    // For crosscheck print the table to the console
    lut.Print();

    // Create FACT camera geometry
    MGeomCamFACT fact;

    // create to camera histograms, one for the colors and one for the patch number
    MHCamera h(fact);
    MHCamera p(fact);
    h.SetTitle("FACT trigger layout in MARS");

    // remove all obsolete stuff around
    h.SetStats(kFALSE);
    h.SetBit(MHCamera::kNoLegend);

    // Number of maximum colors
    Int_t n = 99;

    for (int i=0; i<lut.GetNumRows(); i++)
    {
        // Get the i-th patch
        MArrayI &row = lut.GetRow(i);

        // Check for all colors already used in the neighborhood
        MArrayI col(n+1);
        for (int j=0; j<9; j++)
        {
            if (row[j]==1438 || row[j]==1439)
                continue;

            for (int k=0; k<fact[row[j]].GetNumNeighbors(); k++)
                col[TMath::Nint(h.GetBinContent(fact[row[j]].GetNeighbor(k)+1))]=1;
        }

        // Find the first unused color
        int k = 1;
        for (; k<n+1; k++)
            if (col[k]<0.5)
                break;

        // This is just to "adjust" the colors a bit more to the palette
        // For simplicity (rounding below) the adjustment must not exceed +- 0.5
        // If the number of needed colors is different this needs adjustment!
        Double_t c[99] = { 1.0, 2.4, 3.4, 4.1, 5.0 };

        // Loop over all nine pixles in the patch
        for (int j=0; j<9; j++)
        {
            if (row[j]==1438 || row[j]==1439)
                continue;

            h.SetBinContent(row[j]+1, c[k-1]);  // Color (For a funny symmetry replace it by row[0]%6)
            p.SetBinContent(row[j]+1, i);       // Patch number
            h.SetUsed(row[j]);                  // Switch on that the color is shown
        }
    }

    // Plot the colors and the numbers on top
    h.DrawCopy("");
    p.DrawCopy("same integer");

    // Adjust the canvas attributes
    gPad->SetBorderMode(0);
    gPad->SetFillColor(kWhite);
}
