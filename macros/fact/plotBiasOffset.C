void plotBiasOffset()
{
    PixelMap map;
    if (!map.Read("/home/fact/operation/FACTmap111030.txt"))
        return;

    BiasMap bias;
    if (!bias.Read("/home/fact/operation/GAPDmap_with_spare_module.txt"))
        return;

    MGeomCamFACT fact;

    MHCamera h1(fact);
    MHCamera h2(fact);
    MHCamera h3(fact);
    MHCamera h4(fact);

    h1.SetName("Vnom [V]");
    h2.SetName("Voff [V]");
    h3.SetName("Vnom+Voff [DAC]");
    h4.SetName("Voff [-0.12V/0.19V]");

    h1.SetAllUsed();
    h2.SetAllUsed();
    h3.SetAllUsed();
    h4.SetAllUsed();

    for (int i=0; i<1440; i++)
    {
        // Get entry corresponding to pixel index
        PixelMapEntry &pix = map.index(i);

        // Get entry corresponding to pixel
        BiasMapEntry &ch = bias.hv(pix);

        h1.SetBinContent(i+1,  ch.Vnom); // Volt
        h2.SetBinContent(i+1,  ch.Voff); // Volt
        h3.SetBinContent(i+1, (ch.Vnom+ch.Voff)*4096/90.); // DAC counts
        h4.SetBinContent(i+1,  ch.Voff); // Volt
    }


    TCanvas *c = new TCanvas;
    c->Divide(2,2);

    c->cd(1);
    h1.DrawCopy();

    c->cd(2);
    h2.DrawCopy();

    c->cd(3);
    h3.DrawCopy();

    c->cd(4);
    h4.SetMinMax(-0.12, 0.19);
    h4.DrawCopy();
}
