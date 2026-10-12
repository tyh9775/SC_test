#include <iostream>
#include <vector>
#include <TSpectrum.h>
#include <TFile.h>
#include <TDirectory.h>

std::vector<double> fit_hist( //assuming gaussian
    TCanvas *c,
    TH1D *h,
    TF1 *f,
    TPaveText *box,
    int clr
){
    if (clr==5){
        clr=35;
    }
    c->cd();
    //initial fit to determine the range to use
    h->Fit(f,"Q0");
    //run the fit again over +/- sigma
    double lwr = f->GetParameter(1)-std::abs(f->GetParameter(2)*0.8);
    double upr = f->GetParameter(1)+std::abs(f->GetParameter(2)*0.8);
    
    TF1 *fnew = new TF1("gaus","gaus",lwr,upr);
    h->Fit(fnew,"QR","",lwr,upr);
    fnew->SetLineColor(clr);
    fnew->Draw("SAME");

    
    TText *header = box->AddText(h->GetName());
    header->SetTextFont(42);
    header->SetTextColor(clr);
    header->SetTextAlign(22);

    box->SetFillColor(0);
    box->SetBorderSize(1);
    box->SetTextAlign(12);
    box->SetLineColor(clr);

    
    box->AddLine(0,0,0,0);

    box->AddText(Form("A = %.3e #pm %.3e", fnew->GetParameter(0), fnew->GetParError(0)));
    box->AddText(Form("mean = %.3e #pm %.3e", fnew->GetParameter(1), fnew->GetParError(1)));
    box->AddText(Form("sig = %.3e #pm %.3e", fnew->GetParameter(2), fnew->GetParError(2)));
    box->AddText(Form("Integral (Full)=%.3e",h->Integral()));
    box->AddText(Form("Entries=%.3e",h->GetEntries()));
    box->AddText(Form("Integral (Cut)=%.3e",h->Integral(h->FindFirstBinAbove(0),h->FindLastBinAbove(0))));
    
    box->Draw("SAME");


    return {fnew->GetParameter(1), fnew->GetParError(1)};
}

void fit_hist_alt( //assuming many gaussian peaks
    TCanvas *c,
    TH1D *h,
    TF1 *f,
    TPaveText *box,
    int clr
){
    if (clr==5){
        clr=35;
    }
    c->cd();

    TText *header = box->AddText(h->GetName());
    header->SetTextFont(42);
    header->SetTextColor(clr);
    header->SetTextAlign(22);

    box->SetFillColor(0);
    box->SetBorderSize(1);
    box->SetTextAlign(12);
    box->SetLineColor(clr);

    
    box->AddLine(0,0,0,0);
    
    //initial fit to determine the range to use
    int npeaksmax = 3;
    TSpectrum spec(npeaksmax);
    double sig = 20.0;
    double threshold = 0.10;
    int npeaks = spec.Search(h,sig,"nobackground",threshold);

    double *xspec = spec.GetPositionX();
    //double *yspec = spec.GetPositionY();
    //sort
    std::vector<int> i_spec(npeaks);
    for (int i=0;i<npeaks;i++){
        i_spec[i]=i;
    }
    std::sort(i_spec.begin(),i_spec.end(),[&](int a, int b){return xspec[a]<xspec[b];});
    //std::vector<double> peaks_x {};
    int npeaks_new = std::min(npeaks,npeaksmax);
    for (int i=0;i<npeaks_new;i++){
        double peak_x = xspec[i_spec[i]];
        h->Fit(f,"QR0","",peak_x-20.0,peak_x+20.0);

        double lwr = f->GetParameter(1)-std::abs(f->GetParameter(2)*0.8);
        double upr = f->GetParameter(1)+std::abs(f->GetParameter(2)*0.8);

        TF1 *fnew = new TF1("gaus","gaus",lwr,upr);
        h->Fit(fnew,"QR+","",lwr,upr);
        fnew->SetLineColor(clr);
        fnew->Draw("SAME");
        box->AddText(Form("A%d = %.3e #pm %.3e", i, fnew->GetParameter(0), fnew->GetParError(0)));
        box->AddText(Form("mean%d = %.3e #pm %.3e", i, fnew->GetParameter(1), fnew->GetParError(1)));
        box->AddText(Form("sig%d = %.3e #pm %.3e", i, fnew->GetParameter(2), fnew->GetParError(2)));
    
    }    

    box->Draw("SAME");


    return;
}



void draw_box( //assuming gaussian
    TCanvas *c,
    TH1D *h,
    TPaveText *box,
    int clr,
    double lwr=0.0,
    double upr=0.0
){
    if (clr==5){
        clr=35;
    }
    c->cd();
    //initial fit to determine the range to use    
    TText *header = box->AddText(h->GetName());
    header->SetTextFont(42);
    header->SetTextColor(clr);
    header->SetTextAlign(22);

    box->SetFillColor(0);
    box->SetBorderSize(1);
    box->SetTextAlign(12);
    box->SetLineColor(clr);

    
    box->AddLine(0,0,0,0);
    box->AddText(Form("Entries: %d",static_cast<int>(h->GetEntries())));
    box->AddText(Form("Integral Full: %.2e",static_cast<double>(h->Integral("w"))));
    if (lwr != 0.0  || upr != 0.0){
        box->AddText(Form("Integral [%.2e,%.2e]: %.2e",lwr,upr,static_cast<double>(h->Integral(h->FindBin(lwr),h->FindBin(upr)))));
    }
    

    box->Draw("SAME");


    return;
}

double max_check(
    std::vector<TH1D*> hlist
){
    double max = 0.0; 
    for (size_t i=0;i<hlist.size();i++){
        double max_val = hlist[i]->GetMaximum();
        if (max_val>=max){
            max=max_val;
        }
    }
    return max;
}

std::vector<TPaveText*> box_make(
    int num_box,
    double x1,
    double y1,
    double x2,
    double y2
){

    std::vector<TPaveText*> box_list {};

    for (int i=0;i<num_box;i++){
        TPaveText *box2 = new TPaveText(x1,y1-.1*i,x2,y2-.1*i,"NDC");
        box_list.push_back(box2);
    }
    
    return box_list; 
}


std::vector<double> diff_find(TH1D *hL, TH1D *hR){

    TF1 *fit_left = new TF1("fit_left","gaus",-50,50);
    TF1 *fit_right = new TF1("fit_right","gaus",-50,50);

    hL->Fit(fit_left,"QR0");
    hR->Fit(fit_right,"QR0");

    double lwrl = fit_left->GetParameter(1)-std::abs(fit_left->GetParameter(2)*0.8);
    double uprl = fit_left->GetParameter(1)+std::abs(fit_left->GetParameter(2)*0.8); 

    double lwrr = fit_right->GetParameter(1)-std::abs(fit_right->GetParameter(2)*0.8);
    double uprr = fit_right->GetParameter(1)+std::abs(fit_right->GetParameter(2)*0.8);

    TF1 *fit_left_new = new TF1("fit_left_new","gaus",lwrl,uprl);
    TF1 *fit_right_new = new TF1("fit_right_new","gaus",lwrr,uprr);

    hL->Fit(fit_left_new,"QR0");
    hR->Fit(fit_right_new,"QR0");

    double left = fit_left_new->GetParameter(1);
    double right = fit_right_new->GetParameter(1);

    double left_err = fit_left_new->GetParError(1);
    double right_err = fit_right_new->GetParError(1);

    double diff = left - right;
    double diff_err = pow(pow(left_err,2)+pow(right_err,2),0.5);

    return {diff,diff_err};
}

void write_canvas_2( //assuming gaussian fits
    TCanvas *c, 
    TH1D *h1, 
    TH1D *h2,
    TString title, 
    TFile *f_out,
    TDirectory *dir,
    TF1 *f1=nullptr,
    int clr1=2,
    int clr2=4
){
    f_out->cd();
    dir->cd();
    c->cd();

    h1->SetTitle("");
    h2->SetTitle("");

    h1->SetStats(0);
    h2->SetStats(0);
    
    
    h1->SetLineColor(clr1);
    h2->SetLineColor(clr2);
    h1->SetLineWidth(2);
    h2->SetLineWidth(2);

    double max = h2->GetMaximum();
    if (max<h1->GetMaximum()){
        max = TMath::Max(h1->GetMaximum(), h2->GetMaximum());
    }
    h1->GetYaxis()->SetRangeUser(0, max * 1.1);

    h1->SetTitle(title);

    h1->Draw();
    h2->Draw("SAME");

    TPaveText *box1 = new TPaveText(0.70,0.72,0.90,0.90,"NDC");
    TPaveText *box2 = new TPaveText(0.70,0.52,0.90,0.70,"NDC");
    if (f1){
        fit_hist(c,h1,f1,box1,clr1);
        fit_hist(c,h2,f1,box2,clr2);       
    } else {
        draw_box(c,h1,box1,clr1);
        draw_box(c,h2,box2,clr2);
    }

    gPad->Update();

    c->Update();
    c->Write();
    return;
}

std::vector<double> write_canvas_2_alt( //assuming gaussian fits
    TCanvas *c, 
    TH1D *h1, 
    TH1D *h2,
    TString title, 
    TFile *f_out,
    TDirectory *dir,
    TF1 *f1,
    int clr1=2,
    int clr2=4
){
    f_out->cd();
    dir->cd();
    c->cd();

    h1->SetTitle("");
    h2->SetTitle("");

    h1->SetStats(0);
    h2->SetStats(0);
    
    
    h1->SetLineColor(clr1);
    h2->SetLineColor(clr2);
    h1->SetLineWidth(2);
    h2->SetLineWidth(2);

    double max = h2->GetMaximum();
    if (max<h1->GetMaximum()){
        max = TMath::Max(h1->GetMaximum(), h2->GetMaximum());
    }
    h1->GetYaxis()->SetRangeUser(0, max * 1.1);

    h1->SetTitle(title);

    h1->Draw();
    h2->Draw("SAME");

    TPaveText *box1 = new TPaveText(0.70,0.72,0.90,0.90,"NDC");
    TPaveText *box2 = new TPaveText(0.70,0.52,0.90,0.70,"NDC");
    std::vector<double> h1_stats = fit_hist(c,h1,f1,box1,clr1);
    std::vector<double> h2_stats = fit_hist(c,h2,f1,box2,clr2);       

    gPad->Update();

    c->Update();
    c->Write();

    TCanvas *c1 = new TCanvas(Form("c_%s",h1->GetName()),"Vtx Dist (Reco Tracks)");
    TCanvas *c2 = new TCanvas(Form("c_%s",h2->GetName()),"Vtx Dist (VA Tracks)");
    c1->cd();
    TH1D *h3 = (TH1D*)h1->Clone();
    h3->SetTitle("Vtx Dist Reco");
    h3->Draw();
    TPaveText *box3 = new TPaveText(0.70,0.72,0.90,0.90,"NDC");
    fit_hist(c1,h3,f1,box3,clr1);
    c1->Update();
    c1->Write();

    c2->cd();
    TH1D *h4 = (TH1D*)h2->Clone();
    h4->SetTitle("Vtx Dist VA");
    h4->Draw();
    TPaveText *box4 = new TPaveText(0.70,0.72,0.90,0.90,"NDC");

    fit_hist(c2,h4,f1,box4,clr2);
    c2->Update();
    c2->Write();


    return {h1_stats[0],h1_stats[1],h2_stats[0],h2_stats[1]};
}

void write_canvas_mult( //assuming gaussian fits
    TCanvas *c, 
    TH1D *h1, 
    TH1D *h2,
    TString title, 
    TFile *f_out,
    TDirectory *dir,
    int clr1=2,
    int clr2=4
){
    f_out->cd();
    dir->cd();


    TCanvas *c1 = new TCanvas(Form("c_%s",h1->GetName()),"Multiplicity Dist (Reco Tracks)");
    TCanvas *c2 = new TCanvas(Form("c_%s",h2->GetName()),"Multiplicity Dist (VA Tracks)");
    c->cd();

    h1->SetTitle("");
    h2->SetTitle("");

    h1->SetStats(0);
    h2->SetStats(0);
    
    
    h1->SetLineColor(clr1);
    h2->SetLineColor(clr2);
    h1->SetLineWidth(2);
    h2->SetLineWidth(2);

    double max = h2->GetMaximum();
    if (max<h1->GetMaximum()){
        max = TMath::Max(h1->GetMaximum(), h2->GetMaximum());
    }
    h1->GetYaxis()->SetRangeUser(0, max * 1.1);

    h1->SetTitle(title);

    h1->Draw();
    h2->Draw("SAME");

    TPaveText *box1 = new TPaveText(0.70,0.72,0.90,0.90,"NDC");
    TPaveText *box2 = new TPaveText(0.70,0.52,0.90,0.70,"NDC");
    draw_box(c,h1,box1,clr1,20.0,60.0);
    draw_box(c,h2,box2,clr2,20.0,60.0);     

    gPad->Update();

    c->Update();
    c->Write();

    c1->cd();
    TH1D *h3 = (TH1D*)h1->Clone();
    h3->SetTitle("Mult Dist Reco");
    h3->Draw();
    TPaveText *box3 = new TPaveText(0.70,0.72,0.90,0.90,"NDC");
    draw_box(c1,h3,box3,clr1,20.0,60.0);
    c1->Update();
    c1->Write();

    c2->cd();
    TH1D *h4 = (TH1D*)h2->Clone();
    h4->SetTitle("Mult Dist VA");
    h4->Draw();
    TPaveText *box4 = new TPaveText(0.70,0.72,0.90,0.90,"NDC");

    draw_box(c2,h4,box4,clr2,20.0,60.0);
    c2->Update();
    c2->Write();
    return;
}


std::vector<std::vector<double>> make_comp_vtx(
    int runNum,
    TFile *f_in1,
    TFile *f_in2,
    TFile *f_out,
    TDirectory *parentDir
){
    TH1D *hvx1 = (TH1D*)f_in1->Get("VtxX")->Clone(Form("VtxX_run%4d_nsc",runNum));
    TH1D *hvy1 = (TH1D*)f_in1->Get("VtxY")->Clone(Form("VtxY_run%4d_nsc",runNum));
    TH1D *hvz1 = (TH1D*)f_in1->Get("VtxZ")->Clone(Form("VtxZ_run%4d_nsc",runNum));

    if (hvz1==nullptr || hvz1->GetEntries()==0){
        return {};
    }

    TH1D *hvxva1 = (TH1D*)f_in1->Get("VtxVAX")->Clone(Form("VtxVAX_run%4d_nsc",runNum));
    TH1D *hvyva1 = (TH1D*)f_in1->Get("VtxVAY")->Clone(Form("VtxVAY_run%4d_nsc",runNum));
    TH1D *hvzva1 = (TH1D*)f_in1->Get("VtxVAZ")->Clone(Form("VtxVAZ_run%4d_nsc",runNum));


    TH1D *hvx2 = (TH1D*)f_in2->Get("VtxX")->Clone(Form("VtxX_run%4d_dens",runNum));
    TH1D *hvy2 = (TH1D*)f_in2->Get("VtxY")->Clone(Form("VtxY_run%4d_dens",runNum));
    TH1D *hvz2 = (TH1D*)f_in2->Get("VtxZ")->Clone(Form("VtxZ_run%4d_dens",runNum));

    TH1D *hvxva2 = (TH1D*)f_in2->Get("VtxVAX")->Clone(Form("VtxVAX_run%4d_dens",runNum));
    TH1D *hvyva2 = (TH1D*)f_in2->Get("VtxVAY")->Clone(Form("VtxVAY_run%4d_dens",runNum));
    TH1D *hvzva2 = (TH1D*)f_in2->Get("VtxVAZ")->Clone(Form("VtxVAZ_run%4d_dens",runNum));

    TDirectory *dirVtxX = parentDir->GetDirectory("VtxX");
    TDirectory *dirVtxY = parentDir->GetDirectory("VtxY");
    TDirectory *dirVtxZ = parentDir->GetDirectory("VtxZ");

    if (!dirVtxX) dirVtxX = parentDir->mkdir("VtxX");
    if (!dirVtxY) dirVtxY = parentDir->mkdir("VtxY");
    if (!dirVtxZ) dirVtxZ = parentDir->mkdir("VtxZ");

    TF1 *fit1 = new TF1("fit1", "gaus", -1000, 1000);
    TString ctx1 = "Vtx X Reco and VA Track Comparison (No SC);Vtx (mm);Counts";
    TCanvas *cvx1 = new TCanvas(Form("VtxX_run%4d_comp_nsc",runNum));
    write_canvas_2_alt(cvx1,hvx1,hvxva1,ctx1,f_out,dirVtxX,fit1);
        
    TString cty1 = "Vtx Y Reco and VA Track Comparison (No SC);Vtx (mm);Counts";
    TCanvas *cvy1 = new TCanvas(Form("VtxY_run%4d_comp_nsc",runNum));
    write_canvas_2_alt(cvy1,hvy1,hvyva1,cty1,f_out,dirVtxY,fit1);
    
    TString ctz1 = "Vtx Z Reco and VA Track Comparison (No SC);Vtx (mm);Counts";
    TCanvas *cvz1 = new TCanvas(Form("VtxZ_run%4d_comp_nsc",runNum));
    std::vector<double> vz1 = write_canvas_2_alt(cvz1,hvz1,hvzva1,ctz1,f_out,dirVtxZ,fit1);

    TString ctx2 = "Vtx X Reco and VA Track Comparison (After SC);Vtx (mm);Counts";
    TCanvas *cvx2 = new TCanvas(Form("VtxX_run%4d_comp_dens",runNum));
    write_canvas_2_alt(cvx2,hvx2,hvxva2,ctx2,f_out,dirVtxX,fit1);
        
    TString cty2 = "Vtx Y Reco and VA Track Comparison (After SC);Vtx (mm);Counts";
    TCanvas *cvy2 = new TCanvas(Form("VtxY_run%4d_comp_dens",runNum));
    write_canvas_2_alt(cvy2,hvy2,hvyva2,cty2,f_out,dirVtxY,fit1);
    
    TString ctz2 = "Vtx Z Reco and VA Track Comparison (After SC);Vtx (mm);Counts";
    TCanvas *cvz2 = new TCanvas(Form("VtxZ_run%4d_comp_dens",runNum));
    std::vector<double> vz2 = write_canvas_2_alt(cvz2,hvz2,hvzva2,ctz2,f_out,dirVtxZ,fit1);


    TH1D *hvz1nm = (TH1D*)f_in1->Get("Vtx_no_multZ");
    TH1D *hvz2nm = (TH1D*)f_in2->Get("Vtx_no_multZ");

    if (hvz1nm && hvz2nm){
        hvz1nm->SetName(Form("Vtx_no_multZ_%d",runNum));
        hvz2nm->SetName(Form("Vtx_no_multZ_%d_dens",runNum));
        parentDir->cd();
        hvz1nm->Write();
        hvz2nm->Write();
    }

    return {vz1,vz2};
}


void make_comp_mult(
    int runNum,
    TFile *f_in1,
    TFile *f_in2,
    TFile *f_out,
    TDirectory *parentDir
){
    TH1D *hmult = (TH1D*)f_in1->Get("hmult")->Clone(Form("hmult%4d_nsc",runNum));
    TH1D *hmultVA = (TH1D*)f_in1->Get("hmultVA")->Clone(Form("hmultVA%4d_nsc",runNum));

    TH1D *hmult_dens = (TH1D*)f_in2->Get("hmult")->Clone(Form("hmult%4d_dens",runNum));
    TH1D *hmultVA_dens = (TH1D*)f_in2->Get("hmultVA")->Clone(Form("hmultVA%4d_dens",runNum));

    if (hmult==nullptr || hmult->GetEntries()==0){
        return;
    }

    TDirectory *dirMult = parentDir->GetDirectory("mult");
    if (!dirMult) dirMult = parentDir->mkdir("mult");

    TString ct1 = "Multiplicity From Reco and VA Track Comparison (No SC);Multiplicity;Counts";
    TCanvas *c1 = new TCanvas(Form("Multi_%4d_comp_nsc",runNum));
    write_canvas_mult(c1,hmult,hmultVA,ct1,f_out,dirMult);

    TString ct2 = "Multiplicity From Reco and VA Track Comparison (After SC);Multiplicity;Counts";
    TCanvas *c2 = new TCanvas(Form("Multi_%4d_comp_dens",runNum));
    write_canvas_mult(c2,hmult_dens,hmultVA_dens,ct2,f_out,dirMult);



    
    return;
}



std::vector<std::vector<double>> make_comp_lr(
    int runNum,
    TFile *f_in1,
    TFile *f_in2,
    TFile *f_out,
    TDirectory *parentDir,
    std::string hname,
    TString ctitle
){
    TH1D *hdxnl = (TH1D*)f_in1->Get(Form("%sX_left",hname.c_str()))->Clone(Form("%sX_left_run%4d_nsc",hname.c_str(),runNum));
    TH1D *hdxnr = (TH1D*)f_in1->Get(Form("%sX_right",hname.c_str()))->Clone(Form("%sX_right_run%4d_nsc",hname.c_str(),runNum));

    TH1D *hdxsl = (TH1D*)f_in2->Get(Form("%sX_left",hname.c_str()))->Clone(Form("%sX_left_run%4d_dens",hname.c_str(),runNum));
    TH1D *hdxsr = (TH1D*)f_in2->Get(Form("%sX_right",hname.c_str()))->Clone(Form("%sX_right_run%4d_des",hname.c_str(),runNum));

    TH1D *hdynl = (TH1D*)f_in1->Get(Form("%sY_left",hname.c_str()))->Clone(Form("%sY_left_run%4d_nsc",hname.c_str(),runNum));
    TH1D *hdynr = (TH1D*)f_in1->Get(Form("%sY_right",hname.c_str()))->Clone(Form("%sY_right_run%4d_nsc",hname.c_str(),runNum));

    TH1D *hdysl = (TH1D*)f_in2->Get(Form("%sY_left",hname.c_str()))->Clone(Form("%sY_left_run%4d_dens",hname.c_str(),runNum));
    TH1D *hdysr = (TH1D*)f_in2->Get(Form("%sY_right",hname.c_str()))->Clone(Form("%sY_right_run%4d_dens",hname.c_str(),runNum));


    TF1 *fit1 = new TF1("fit1", "gaus", -100, 100);


    TDirectory *dirDXLR = parentDir->GetDirectory("DXLR");
    TDirectory *dirDYLR = parentDir->GetDirectory("DYLR");
    TDirectory *dirDXLRsc = parentDir->GetDirectory("DXLRsc");
    TDirectory *dirDYLRsc = parentDir->GetDirectory("DYLRsc");

    if (!dirDXLR) dirDXLR = parentDir->mkdir("DXLR");
    if (!dirDYLR) dirDYLR = parentDir->mkdir("DYLR");
    if (!dirDXLRsc) dirDXLRsc = parentDir->mkdir("DXLRsc");
    if (!dirDYLRsc) dirDYLRsc = parentDir->mkdir("DYLRsc");

    TString ctxn = Form("%s_{X} Left and Right Comparison (No Space Charge);#Delta V_{X} (mm);Counts",ctitle.Data());
    TCanvas *cdxn = new TCanvas(Form("%sX_run%4d_nsc_comp",hname.c_str(),runNum));
    write_canvas_2(cdxn,hdxnl,hdxnr,ctxn,f_out,dirDXLR,fit1);

    TString ctxs = Form("%s_{X} Left and Right Comparison (Test Dens);#Delta V_{X} (mm);Counts",ctitle.Data());
    TCanvas *cdxs = new TCanvas(Form("%sX_run%4d_dens_comp",hname.c_str(),runNum));
    write_canvas_2(cdxs,hdxsl,hdxsr,ctxs,f_out,dirDXLRsc,fit1);


    TString ctyn = Form("%s_{Y} Left and Right Comparison (No Space Charge);#Delta V_{Y} (mm);Counts",ctitle.Data());
    TCanvas *cdyn = new TCanvas(Form("%sY_run%4d_nsc_comp",hname.c_str(),runNum));
    write_canvas_2(cdyn,hdynl,hdynr,ctyn,f_out,dirDYLR,fit1);

    TString ctys = Form("%s_{Y} Left and Right Comparison (Test Dens);#Delta V_{Y} (mm);Counts",ctitle.Data());
    TCanvas *cdys = new TCanvas(Form("%sY_run%4d_dens_comp",hname.c_str(),runNum));
    write_canvas_2(cdys,hdysl,hdysr,ctys,f_out,dirDYLRsc,fit1);

    std::vector<double> dxnlr = diff_find(hdxnl,hdxnr);
    std::vector<double> dxslr = diff_find(hdxsl,hdxsr);
    

    std::vector<double> dynlr = diff_find(hdynl,hdynr);
    std::vector<double> dyslr = diff_find(hdysl,hdysr);

    return {dxnlr,dxslr,dynlr,dyslr};
}




void draw_mult_graph(
    TCanvas *c,
    TMultiGraph *g,
    TLegend *legend,
    TFile *fo,
    bool fit=false,
    bool fit_comp=false
){

    c->cd();
    gPad->SetGrid();
    gStyle->SetGridStyle(3);
    gStyle->SetGridColor(kGray);
    g->Draw("AP");

    if (legend){
        legend->Draw("SAME");
    }
    
    if (fit){
        TF1 *f0 = new TF1("f0","pol0");
        g->Fit(f0,"Q+");
        f0->SetLineColor(kRed);
        f0->Draw("SAME");
        TPaveText *box0 = new TPaveText(0.70,0.82,0.90,0.90,"NDC");
        box0->SetFillColor(0);
        box0->SetBorderSize(1);
        box0->SetTextAlign(12);
        box0->SetLineColor(kRed);
        box0->AddText(Form("p0 = %.3e #pm %.3e", f0->GetParameter(0), f0->GetParError(0)));
        box0->Draw("SAME");
    }
    if (fit_comp){
        TF1 *f1 = new TF1("f1","pol1",10,60);
        g->Fit(f1,"Q+");
        f1->SetLineColor(kBlue);
        f1->Draw("SAME");
        TPaveText *box1 = new TPaveText(0.70,0.72,0.90,0.80,"NDC");
        box1->SetFillColor(0);
        box1->SetBorderSize(1);
        box1->SetTextAlign(12);
        box1->SetLineColor(kBlue);
        box1->AddText(Form("p0 = %.3e #pm %.3e", f1->GetParameter(0), f1->GetParError(0)));
        box1->AddText(Form("p1 = %.3e #pm %.3e", f1->GetParameter(1), f1->GetParError(1)));
        box1->Draw("SAME");
    }   
    fo->cd();
    c->Write();
    return;
}




void write_canvas_proj( //assuming gaussian fits
    double dedx,
    TCanvas *c, 
    TH2D *hA, 
    TH2D *hB,
    TString title, 
    TFile *f_out,
    TDirectory *dir=nullptr,
    std::string s1="nsc",
    std::string s2="sc",
    TF1 *f1 = nullptr,
    int clr1=2,
    int clr2=4
){
    f_out->cd();
    if (dir){
        dir->cd();
    }
    c->cd();
    /* gStyle->SetOptStat(0);
    gStyle->SetOptFit(0); */

    int y1 = hA->GetYaxis()->FindBin(dedx-10.0);
    int y2 = hB->GetYaxis()->FindBin(dedx+10.0);


    TH1D *h1 = hA->ProjectionX(Form("proj_%dpm10_%s",static_cast<int>(dedx),s1.c_str()),y1,y2);
    TH1D *h2 = hB->ProjectionX(Form("proj_%dpm10_%s",static_cast<int>(dedx),s2.c_str()),y1,y2);

    h1->Rebin(10);
    h2->Rebin(10);

    h1->SetTitle("");
    h2->SetTitle("");

    h1->SetStats(0);
    h2->SetStats(0);
    
    
    h1->SetLineColor(clr1);
    h2->SetLineColor(clr2);
    h1->SetLineWidth(2);
    h2->SetLineWidth(2);

    //double max = TMath::Max(h1->GetMaximum(), h2->GetMaximum());
    double max = h2->GetMaximum();
    if (max<h1->GetMaximum()){
        max = TMath::Max(h1->GetMaximum(), h2->GetMaximum());
    }
    h1->GetYaxis()->SetRangeUser(0, max * 1.1);

    h1->SetTitle(title);

    h1->Draw();
    h2->Draw("SAME");


    
    TPaveText *box1 = new TPaveText(0.70,0.72,0.90,0.90,"NDC");
    TPaveText *box2 = new TPaveText(0.70,0.52,0.90,0.70,"NDC");
    if (f1){
        fit_hist_alt(c,h1,f1,box1,clr1);
        fit_hist_alt(c,h2,f1,box2,clr2);       
    } else {
        draw_box(c,h1,box1,clr1);
        draw_box(c,h2,box2,clr2);
    }


    gPad->Update();

    /* TPaveLabel *ct = new TPaveLabel(0.1, 0.95, 0.9, 1.0, title, "NDC");
    ct->SetFillColor(0);
    ct->SetBorderSize(0);
    ct->SetTextSize(0.6);
    ct->Draw(); */

    c->Update();
    c->Write();
    //delete ct;
    return;
}


void comp_mnt_proj(
    int runNum,
    TFile *f_in1,
    TFile *f_in2,
    TFile *f_out,
    TDirectory *parentDir,
    std::string hname,
    TString ctitle,
    std::vector<double> dedx_list = {40.0,80.0,120.0,160.0,200.0,240.0,280.0,320.0,360.0,400.0}
){
    TH2D *hn = (TH2D*)f_in1->Get(Form("%s",hname.c_str()))->Clone(Form("%s_nsc",hname.c_str()));
    hn->RebinX(2);
    TH2D *hnl = (TH2D*)f_in1->Get(Form("%sL",hname.c_str()))->Clone(Form("%sL_nsc",hname.c_str()));
    hnl->RebinX(2);
    TH2D *hnr = (TH2D*)f_in1->Get(Form("%sR",hname.c_str()))->Clone(Form("%sR_nsc",hname.c_str()));
    hnr->RebinX(2);

    TH2D *hnlm = (TH2D*)f_in1->Get(Form("%sMntL",hname.c_str()))->Clone(Form("%sMntL_nsc",hname.c_str()));
    hnlm->RebinX(2);
    TH2D *hnrm = (TH2D*)f_in1->Get(Form("%sMntR",hname.c_str()))->Clone(Form("%sMntR_nsc",hname.c_str()));    
    hnrm->RebinX(2);

    TH2D *hnlp = (TH2D*)f_in1->Get(Form("%sPhiL",hname.c_str()))->Clone(Form("%sPhiL_nsc",hname.c_str()));
    hnlp->RebinX(2);
    TH2D *hnrp = (TH2D*)f_in1->Get(Form("%sPhiR",hname.c_str()))->Clone(Form("%sPhiR_nsc",hname.c_str()));
    hnrp->RebinX(2);
    
    TH2D *hnlmp = (TH2D*)f_in1->Get(Form("%sMntPhiL",hname.c_str()))->Clone(Form("%sMntPhiL_nsc",hname.c_str()));
    hnlmp->RebinX(2);
    TH2D *hnrmp = (TH2D*)f_in1->Get(Form("%sMntPhiR",hname.c_str()))->Clone(Form("%sMntPhiR_nsc",hname.c_str()));
    hnrmp->RebinX(2);


    TH2D *hs = (TH2D*)f_in2->Get(Form("%s",hname.c_str()))->Clone(Form("%s_dens",hname.c_str()));
    hs->RebinX(2);
    TH2D *hsl = (TH2D*)f_in2->Get(Form("%sL",hname.c_str()))->Clone(Form("%sL_dens",hname.c_str()));
    hsl->RebinX(2);
    TH2D *hsr = (TH2D*)f_in2->Get(Form("%sR",hname.c_str()))->Clone(Form("%sR_dens",hname.c_str()));
    hsr->RebinX(2);


    TH2D *hslm = (TH2D*)f_in2->Get(Form("%sMntL",hname.c_str()))->Clone(Form("%sMntL_dens",hname.c_str()));
    hslm->RebinX(2);
    TH2D *hsrm = (TH2D*)f_in2->Get(Form("%sMntR",hname.c_str()))->Clone(Form("%sMntR_dens",hname.c_str()));    
    hsrm->RebinX(2);

    TH2D *hslp = (TH2D*)f_in2->Get(Form("%sPhiL",hname.c_str()))->Clone(Form("%sPhiL_dens",hname.c_str()));
    hslp->RebinX(2);
    TH2D *hsrp = (TH2D*)f_in2->Get(Form("%sPhiR",hname.c_str()))->Clone(Form("%sPhiR_dens",hname.c_str()));
    hsrp->RebinX(2);
    
    TH2D *hslmp = (TH2D*)f_in2->Get(Form("%sMntPhiL",hname.c_str()))->Clone(Form("%sMntPhiL_dens",hname.c_str()));
    hslmp->RebinX(2);
    TH2D *hsrmp = (TH2D*)f_in2->Get(Form("%sMntPhiR",hname.c_str()))->Clone(Form("%sMntPhiR_dens",hname.c_str()));
    hsrmp->RebinX(2);


    

    TH2D *hnh = (TH2D*)f_in1->Get(Form("%s_high",hname.c_str()))->Clone(Form("%s_high_nsc",hname.c_str()));
    hnh->RebinX(2);
    TH2D *hnlh = (TH2D*)f_in1->Get(Form("%sL_high",hname.c_str()))->Clone(Form("%sL_high_nsc",hname.c_str()));
    hnlh->RebinX(2);
    TH2D *hnrh = (TH2D*)f_in1->Get(Form("%sR_high",hname.c_str()))->Clone(Form("%sR_high_nsc",hname.c_str()));
    hnrh->RebinX(2);


    TH2D *hnlmh = (TH2D*)f_in1->Get(Form("%sMntL_high",hname.c_str()))->Clone(Form("%sMntL_high_nsc",hname.c_str()));
    hnlmh->RebinX(2);
    TH2D *hnrmh = (TH2D*)f_in1->Get(Form("%sMntR_high",hname.c_str()))->Clone(Form("%sMntR_high_nsc",hname.c_str()));    
    hnrmh->RebinX(2);

    TH2D *hnlph = (TH2D*)f_in1->Get(Form("%sPhiL_high",hname.c_str()))->Clone(Form("%sPhiL_high_nsc",hname.c_str()));
    hnlph->RebinX(2);
    TH2D *hnrph = (TH2D*)f_in1->Get(Form("%sPhiR_high",hname.c_str()))->Clone(Form("%sPhiR_high_nsc",hname.c_str()));
    hnrph->RebinX(2);
    
    TH2D *hnlmph = (TH2D*)f_in1->Get(Form("%sMntPhiL_high",hname.c_str()))->Clone(Form("%sMntPhiL_high_nsc",hname.c_str()));
    hnlmph->RebinX(2);
    TH2D *hnrmph = (TH2D*)f_in1->Get(Form("%sMntPhiR_high",hname.c_str()))->Clone(Form("%sMntPhiR_high_nsc",hname.c_str()));
    hnrmph->RebinX(2);



    TH2D *hsh = (TH2D*)f_in2->Get(Form("%s_high",hname.c_str()))->Clone(Form("%s_high_dens",hname.c_str()));
    hsh->RebinX(2);
    TH2D *hslh = (TH2D*)f_in2->Get(Form("%sL_high",hname.c_str()))->Clone(Form("%sL_high_dens",hname.c_str()));
    hslh->RebinX(2);
    TH2D *hsrh = (TH2D*)f_in2->Get(Form("%sR_high",hname.c_str()))->Clone(Form("%sR_high_dens",hname.c_str()));
    hsrh->RebinX(2);



    TH2D *hslmh = (TH2D*)f_in2->Get(Form("%sMntL_high",hname.c_str()))->Clone(Form("%sMntL_high_dens",hname.c_str()));
    hslmh->RebinX(2);
    TH2D *hsrmh = (TH2D*)f_in2->Get(Form("%sMntR_high",hname.c_str()))->Clone(Form("%sMntR_high_dens",hname.c_str()));    
    hsrmh->RebinX(2);

    TH2D *hslph = (TH2D*)f_in2->Get(Form("%sPhiL_high",hname.c_str()))->Clone(Form("%sPhiL_high_dens",hname.c_str()));
    hslph->RebinX(2);
    TH2D *hsrph = (TH2D*)f_in2->Get(Form("%sPhiR_high",hname.c_str()))->Clone(Form("%sPhiR_high_dens",hname.c_str()));
    hsrph->RebinX(2);
    
    TH2D *hslmph = (TH2D*)f_in2->Get(Form("%sMntPhiL_high",hname.c_str()))->Clone(Form("%sMntPhiL_high_dens",hname.c_str()));
    hslmph->RebinX(2);
    TH2D *hsrmph = (TH2D*)f_in2->Get(Form("%sMntPhiR_high",hname.c_str()))->Clone(Form("%sMntPhiR_high_dens",hname.c_str()));
    hsrmph->RebinX(2);




    TH2D *hnlw = (TH2D*)f_in1->Get(Form("%s_low",hname.c_str()))->Clone(Form("%s_low_nsc",hname.c_str()));
    hnlw->RebinX(2);
    TH2D *hnllw = (TH2D*)f_in1->Get(Form("%sL_low",hname.c_str()))->Clone(Form("%sL_low_nsc",hname.c_str()));
    hnllw->RebinX(2);
    TH2D *hnrlw = (TH2D*)f_in1->Get(Form("%sR_low",hname.c_str()))->Clone(Form("%sR_low_nsc",hname.c_str()));
    hnrlw->RebinX(2);

    TH2D *hnlmlw = (TH2D*)f_in1->Get(Form("%sMntL_low",hname.c_str()))->Clone(Form("%sMntL_low_nsc",hname.c_str()));
    hnlmlw->RebinX(2);
    TH2D *hnrmlw = (TH2D*)f_in1->Get(Form("%sMntR_low",hname.c_str()))->Clone(Form("%sMntR_low_nsc",hname.c_str()));    
    hnrmlw->RebinX(2);

    TH2D *hnlplw = (TH2D*)f_in1->Get(Form("%sPhiL_low",hname.c_str()))->Clone(Form("%sPhiL_low_nsc",hname.c_str()));
    hnlplw->RebinX(2);
    TH2D *hnrplw = (TH2D*)f_in1->Get(Form("%sPhiR_low",hname.c_str()))->Clone(Form("%sPhiR_low_nsc",hname.c_str()));
    hnrplw->RebinX(2);
    
    TH2D *hnlmplw = (TH2D*)f_in1->Get(Form("%sMntPhiL_low",hname.c_str()))->Clone(Form("%sMntPhiL_low_nsc",hname.c_str()));
    hnlmplw->RebinX(2);
    TH2D *hnrmplw = (TH2D*)f_in1->Get(Form("%sMntPhiR_low",hname.c_str()))->Clone(Form("%sMntPhiR_low_nsc",hname.c_str()));
    hnrmplw->RebinX(2);


    TH2D *hslw = (TH2D*)f_in2->Get(Form("%s_low",hname.c_str()))->Clone(Form("%s_low_dens",hname.c_str()));
    hslw->RebinX(2);
    TH2D *hsllw = (TH2D*)f_in2->Get(Form("%sL_low",hname.c_str()))->Clone(Form("%sL_low_dens",hname.c_str()));
    hsllw->RebinX(2);
    TH2D *hsrlw = (TH2D*)f_in2->Get(Form("%sR_low",hname.c_str()))->Clone(Form("%sR_low_dens",hname.c_str()));
    hsrlw->RebinX(2);


    TH2D *hslmlw = (TH2D*)f_in2->Get(Form("%sMntL_low",hname.c_str()))->Clone(Form("%sMntL_low_dens",hname.c_str()));
    hslmlw->RebinX(2);
    TH2D *hsrmlw = (TH2D*)f_in2->Get(Form("%sMntR_low",hname.c_str()))->Clone(Form("%sMntR_low_dens",hname.c_str()));    
    hsrmlw->RebinX(2);

    TH2D *hslplw = (TH2D*)f_in2->Get(Form("%sPhiL_low",hname.c_str()))->Clone(Form("%sPhiL_low_dens",hname.c_str()));
    hslplw->RebinX(2);
    TH2D *hsrplw = (TH2D*)f_in2->Get(Form("%sPhiR_low",hname.c_str()))->Clone(Form("%sPhiR_low_dens",hname.c_str()));
    hsrplw->RebinX(2);
    
    TH2D *hslmplw = (TH2D*)f_in2->Get(Form("%sMntPhiL_low",hname.c_str()))->Clone(Form("%sMntPhiL_low_dens",hname.c_str()));
    hslmplw->RebinX(2);
    TH2D *hsrmplw = (TH2D*)f_in2->Get(Form("%sMntPhiR_low",hname.c_str()))->Clone(Form("%sMntPhiR_low_dens",hname.c_str()));
    hsrmplw->RebinX(2);

    TDirectory *dirBoth = parentDir->GetDirectory("Both");
    TDirectory *dirL = parentDir->GetDirectory("L");
    TDirectory *dirR = parentDir->GetDirectory("R");

    TDirectory *dirLR = parentDir->GetDirectory("LR");
    TDirectory *dirMntLR = parentDir->GetDirectory("MntLR");
    TDirectory *dirPhiLR = parentDir->GetDirectory("PhiLR");
    TDirectory *dirMntPhiLR = parentDir->GetDirectory("MntPhiLR");

    TDirectory *dirLRsc = parentDir->GetDirectory("LRsc");
    TDirectory *dirMntLRsc = parentDir->GetDirectory("MntLRsc");
    TDirectory *dirPhiLRsc = parentDir->GetDirectory("PhiLRsc");
    TDirectory *dirMntPhiLRsc = parentDir->GetDirectory("MntPhiLRsc");

    if (!dirBoth) dirBoth = parentDir->mkdir("Both");
    if (!dirL) dirL = parentDir->mkdir("L");
    if (!dirR) dirR = parentDir->mkdir("R");

    if (!dirLR) dirLR = parentDir->mkdir("LR");
    if (!dirMntLR) dirMntLR = parentDir->mkdir("MntLR");
    if (!dirPhiLR) dirPhiLR = parentDir->mkdir("PhiLR");
    if (!dirMntPhiLR) dirMntPhiLR = parentDir->mkdir("MntPhiLR");
    
    if (!dirLRsc) dirLRsc = parentDir->mkdir("LRsc");
    if (!dirMntLRsc) dirMntLRsc = parentDir->mkdir("MntLRsc");
    if (!dirPhiLRsc) dirPhiLRsc = parentDir->mkdir("PhiLRsc");
    if (!dirMntPhiLRsc) dirMntPhiLRsc = parentDir->mkdir("MntPhiLRsc");



    for (size_t i=0;i<dedx_list.size();i++){
        f_out->cd();
        int dedx=static_cast<int>(dedx_list[i]);
        TString ct = Form("%s No SC and Test Density (All Angles, Both Dir);Momentum (MeV/c);Counts",ctitle.Data());
        TCanvas *c = new TCanvas(Form("%s_projX_%d_run%4d_comp",hname.c_str(),dedx,runNum));
        write_canvas_proj(dedx_list[i],c,hn,hs,ct,f_out,dirBoth);

        TString ctl = Form("%s No SC and Test Density (All Angles, Left);Momentum (MeV/c);Counts",ctitle.Data());
        TCanvas *cl = new TCanvas(Form("%s_projX_%d_run%4d_comp_l",hname.c_str(),dedx,runNum));
        write_canvas_proj(dedx_list[i],cl,hnl,hsl,ctl,f_out,dirL);

        TString ctr = Form("%s No SC and Test Density (All Angles, Right);Momentum (MeV/c);Counts",ctitle.Data());
        TCanvas *cr = new TCanvas(Form("%s_projX_%d_run%4d_comp_r",hname.c_str(),dedx,runNum));
        write_canvas_proj(dedx_list[i],cr,hnr,hsr,ctr,f_out,dirR);



        TString cth = Form("%s No SC and Test Density (40+ Deg);Momentum (MeV/c);Counts",ctitle.Data());
        TCanvas *ch = new TCanvas(Form("%s_projX_%d_run%4d_comp_high",hname.c_str(),dedx,runNum));
        write_canvas_proj(dedx_list[i],ch,hnh,hsh,cth,f_out,dirBoth);

        TString ctlh = Form("%s No SC and Test Density (40+ Deg, Left);Momentum (MeV/c);Counts",ctitle.Data());
        TCanvas *clh =  new TCanvas(Form("%s_projX_%d_run%4d_comp_l_high",hname.c_str(),dedx,runNum));
        write_canvas_proj(dedx_list[i],clh,hnlh,hslh,ctlh,f_out,dirL);

        TString ctrh = Form("%s No SC and Test Density (40+ Deg, Right);Momentum (MeV/c);Counts",ctitle.Data());
        TCanvas *crh =  new TCanvas(Form("%s_projX_%d_run%4d_comp_r_high",hname.c_str(),dedx,runNum));
        write_canvas_proj(dedx_list[i],crh,hnrh,hsrh,ctrh,f_out,dirR);



        TString ctlw = Form("%s No SC and Test Density (40- Deg);Momentum (MeV/c);Counts",ctitle.Data());
        TCanvas *clw =  new TCanvas(Form("%s_projX_%d_run%4d_comp_low",hname.c_str(),dedx,runNum));
        write_canvas_proj(dedx_list[i],clw,hnlw,hslw,ctlw,f_out,dirBoth);

        TString ctllw = Form("%s No SC and Test Density (40- Deg, Left);Momentum (MeV/c);Counts",ctitle.Data());
        TCanvas *cllw = new TCanvas(Form("%s_projX_%d_run%4d_comp_l_low",hname.c_str(),dedx,runNum));
        write_canvas_proj(dedx_list[i],cllw,hnllw,hsllw,ctllw,f_out,dirL);

        TString ctrlw = Form("%s No SC and Test Density (40- Deg, Right);Momentum (MeV/c);Counts",ctitle.Data());
        TCanvas *crlw = new TCanvas(Form("%s_projX_%d_run%4d_comp_r_low",hname.c_str(),dedx,runNum));
        write_canvas_proj(dedx_list[i],crlw,hnrlw,hsrlw,ctrlw,f_out,dirR);


        /////////////////
        TString ctlr = Form("%s Left and Right Comparison, No SC Correction (All Angles);Momentum (MeV/c);Counts",ctitle.Data());
        TCanvas *clr = new TCanvas(Form("%s_projX_%d_run%4d_comp_lr",hname.c_str(),dedx,runNum));
        write_canvas_proj(dedx_list[i],clr,hnl,hnr,ctlr,f_out,dirLR,"left","right");

        TF1 *f1 = new TF1("f1","gaus",0.0,2000.0);
        TString ctlrh = Form("%s Left and Right Comparison, No SC Correction (40+ Deg);Momentum (MeV/c);Counts",ctitle.Data());
        TCanvas *clrh = new TCanvas(Form("%s_projX_%d_run%4d_comp_lr_high",hname.c_str(),dedx,runNum));
        write_canvas_proj(dedx_list[i],clrh,hnlh,hnrh,ctlrh,f_out,dirLR,"left","right");

        TString ctlrl = Form("%s Left and Right Comparison, No SC Correction (40- Deg);Momentum (MeV/c);Counts",ctitle.Data());
        TCanvas *clrl = new TCanvas(Form("%s_projX_%d_run%4d_comp_lr_low",hname.c_str(),dedx,runNum));
        write_canvas_proj(dedx_list[i],clrl,hnllw,hnrlw,ctlrl,f_out,dirLR,"left","right");



        TString ctlrm = Form("%s Left and Right Comparison, No SC Correction (All Angles, Mnt X);Momentum (MeV/c);Counts",ctitle.Data());
        TCanvas *clrm = new TCanvas(Form("%s_projX_%d_run%4d_comp_lr_mnt",hname.c_str(),dedx,runNum));
        write_canvas_proj(dedx_list[i],clrm,hnlm,hnrm,ctlrm,f_out,dirMntLR,"left","right");

        TString ctlrmh = Form("%s Left and Right Comparison, No SC Correction (40+ Deg, Mnt X);Momentum (MeV/c);Counts",ctitle.Data());
        TCanvas *clrmh = new TCanvas(Form("%s_projX_%d_run%4d_comp_lr_mnt_high",hname.c_str(),dedx,runNum));
        write_canvas_proj(dedx_list[i],clrmh,hnlmh,hnrmh,ctlrmh,f_out,dirMntLR,"left","right");

        TString ctlrml = Form("%s Left and Right Comparison, No SC Correction (40- Deg, Mnt X);Momentum (MeV/c);Counts",ctitle.Data());
        TCanvas *clrml = new TCanvas(Form("%s_projX_%d_run%4d_comp_lr_mnt_low",hname.c_str(),dedx,runNum));
        write_canvas_proj(dedx_list[i],clrml,hnlmlw,hnrmlw,ctlrml,f_out,dirMntLR,"left","right");



        TString ctlrp = Form("%s Left and Right Comparison, No SC Correction (All Angles, Phi Cut);Momentum (MeV/c);Counts",ctitle.Data());
        TCanvas *clrp = new TCanvas(Form("%s_projX_%d_run%4d_comp_lr_phi",hname.c_str(),dedx,runNum));
        write_canvas_proj(dedx_list[i],clrp,hnlp,hnrp,ctlrp,f_out,dirPhiLR,"left","right");

        TString ctlrph = Form("%s Left and Right Comparison, No SC Correction (40+ Deg, Phi Cut);Momentum (MeV/c);Counts",ctitle.Data());
        TCanvas *clrph = new TCanvas(Form("%s_projX_%d_run%4d_comp_lr_phi_high",hname.c_str(),dedx,runNum));
        write_canvas_proj(dedx_list[i],clrph,hnlph,hnrph,ctlrph,f_out,dirPhiLR,"left","right");

        TString ctlrpl = Form("%s Left and Right Comparison, No SC Correction (40- Deg, Phi Cut);Momentum (MeV/c);Counts",ctitle.Data());
        TCanvas *clrpl = new TCanvas(Form("%s_projX_%d_run%4d_comp_lr_phi_low",hname.c_str(),dedx,runNum));
        write_canvas_proj(dedx_list[i],clrpl,hnlplw,hnrplw,ctlrpl,f_out,dirPhiLR,"left","right");        


        TString ctlrmp = Form("%s Left and Right Comparison, No SC Correction (All Angles, Mnt X, Phi Cut);Momentum (MeV/c);Counts",ctitle.Data());
        TCanvas *clrmp = new TCanvas(Form("%s_projX_%d_run%4d_comp_lr_mnt_phi",hname.c_str(),dedx,runNum));
        write_canvas_proj(dedx_list[i],clrmp,hnlmp,hnrmp,ctlrmp,f_out,dirMntPhiLR,"left","right");

        TString ctlrmph = Form("%s Left and Right Comparison, No SC Correction (40+ Deg, Mnt X, Phi Cut);Momentum (MeV/c);Counts",ctitle.Data());
        TCanvas *clrmph = new TCanvas(Form("%s_projX_%d_run%4d_comp_lr_mnt_phi_high",hname.c_str(),dedx,runNum));
        write_canvas_proj(dedx_list[i],clrmph,hnlmph,hnrmph,ctlrmph,f_out,dirMntPhiLR,"left","right");

        TString ctlrmpl = Form("%s Left and Right Comparison, No SC Correction (40- Deg, Mnt X, Phi Cut);Momentum (MeV/c);Counts",ctitle.Data());
        TCanvas *clrmpl = new TCanvas(Form("%s_projX_%d_run%4d_comp_lr_mnt_phi_low",hname.c_str(),dedx,runNum));
        write_canvas_proj(dedx_list[i],clrmpl,hnlmplw,hnrmplw,ctlrmpl,f_out,dirMntPhiLR,"left","right");

        ///////////////



        TString ctlrs = Form("%s Left and Right Comparison, Test Density (All Angles);Momentum (MeV/c);Counts",ctitle.Data());
        TCanvas *clrs = new TCanvas(Form("%s_projX_%d_run%4d_comp_lr_dens",hname.c_str(),dedx,runNum));
        write_canvas_proj(dedx_list[i],clrs,hsl,hsr,ctlrs,f_out,dirLRsc,"left","right");

        TString ctlrhs = Form("%s Left and Right Comparison, Test Density (40+ Deg);Momentum (MeV/c);Counts",ctitle.Data());
        TCanvas *clrhs = new TCanvas(Form("%s_projX_%d_run%4d_comp_lr_high_dens",hname.c_str(),dedx,runNum));
        write_canvas_proj(dedx_list[i],clrhs,hslh,hsrh,ctlrhs,f_out,dirLRsc,"left","right");

        TString ctlrls = Form("%s Left and Right Comparison, Test Density (40- Deg);Momentum (MeV/c);Counts",ctitle.Data());
        TCanvas *clrls = new TCanvas(Form("%s_projX_%d_run%4d_comp_lr_low_dens",hname.c_str(),dedx,runNum));
        write_canvas_proj(dedx_list[i],clrls,hsllw,hsrlw,ctlrls,f_out,dirLRsc,"left","right");



        TString ctlrms = Form("%s Left and Right Comparison, Test Density (All Angles, Mnt X);Momentum (MeV/c);Counts",ctitle.Data());
        TCanvas *clrms = new TCanvas(Form("%s_projX_%d_run%4d_comp_lr_mnt_dens",hname.c_str(),dedx,runNum));
        write_canvas_proj(dedx_list[i],clrms,hslm,hsrm,ctlrms,f_out,dirMntLRsc,"left","right");

        TString ctlrmhs = Form("%s Left and Right Comparison, Test Density (40+ Deg, Mnt X);Momentum (MeV/c);Counts",ctitle.Data());
        TCanvas *clrmhs = new TCanvas(Form("%s_projX_%d_run%4d_comp_lr_mnt_high_dens",hname.c_str(),dedx,runNum));
        write_canvas_proj(dedx_list[i],clrmhs,hslmh,hsrmh,ctlrmhs,f_out,dirMntLRsc,"left","right");

        TString ctlrmls = Form("%s Left and Right Comparison, Test Density (40- Deg, Mnt X);Momentum (MeV/c);Counts",ctitle.Data());
        TCanvas *clrmls = new TCanvas(Form("%s_projX_%d_run%4d_comp_lr_mnt_low_dens",hname.c_str(),dedx,runNum));
        write_canvas_proj(dedx_list[i],clrmls,hslmlw,hsrmlw,ctlrmls,f_out,dirMntLRsc,"left","right");



        TString ctlrps = Form("%s Left and Right Comparison, Test Density (All Angles, Phi Cut);Momentum (MeV/c);Counts",ctitle.Data());
        TCanvas *clrps = new TCanvas(Form("%s_projX_%d_run%4d_comp_lr_phi_dens",hname.c_str(),dedx,runNum));
        write_canvas_proj(dedx_list[i],clrps,hslp,hsrp,ctlrps,f_out,dirPhiLRsc,"left","right");

        TString ctlrphs = Form("%s Left and Right Comparison, Test Density (40+ Deg, Phi Cut);Momentum (MeV/c);Counts",ctitle.Data());
        TCanvas *clrphs = new TCanvas(Form("%s_projX_%d_run%4d_comp_lr_phi_high_dens",hname.c_str(),dedx,runNum));
        write_canvas_proj(dedx_list[i],clrphs,hslph,hsrph,ctlrphs,f_out,dirPhiLRsc,"left","right");

        TString ctlrpls = Form("%s Left and Right Comparison, Test Density (40- Deg, Phi Cut);Momentum (MeV/c);Counts",ctitle.Data());
        TCanvas *clrpls = new TCanvas(Form("%s_projX_%d_run%4d_comp_lr_phi_low_dens",hname.c_str(),dedx,runNum));
        write_canvas_proj(dedx_list[i],clrpls,hslplw,hsrplw,ctlrpls,f_out,dirPhiLRsc,"left","right");        


        TString ctlrmps = Form("%s Left and Right Comparison, Test Density (All Angles, Mnt X, Phi Cut);Momentum (MeV/c);Counts",ctitle.Data());
        TCanvas *clrmps = new TCanvas(Form("%s_projX_%d_run%4d_comp_lr_mnt_phi_dens",hname.c_str(),dedx,runNum));
        write_canvas_proj(dedx_list[i],clrmps,hslmp,hsrmp,ctlrmps,f_out,dirMntPhiLRsc,"left","right");

        TString ctlrmphs = Form("%s Left and Right Comparison, Test Density (40+ Deg, Mnt X, Phi Cut);Momentum (MeV/c);Counts",ctitle.Data());
        TCanvas *clrmphs = new TCanvas(Form("%s_projX_%d_run%4d_comp_lr_mnt_phi_high_dens",hname.c_str(),dedx,runNum));
        write_canvas_proj(dedx_list[i],clrmphs,hslmph,hsrmph,ctlrmphs,f_out,dirMntPhiLRsc,"left","right");

        TString ctlrmpls = Form("%s Left and Right Comparison, Test Density (40- Deg, Mnt X, Phi Cut);Momentum (MeV/c);Counts",ctitle.Data());
        TCanvas *clrmpls = new TCanvas(Form("%s_projX_%d_run%4d_comp_lr_mnt_phi_low_dens",hname.c_str(),dedx,runNum));
        write_canvas_proj(dedx_list[i],clrmpl,hslmplw,hsrmplw,ctlrmpl,f_out,dirMntPhiLRsc,"left","right");        
    }


    return;
}

void compare(
    int runNum = 1642,
    std::string input_name1 = "dens_no_dens.root",
    std::string input_name2 = "dens_dens.root",
    std::string output_name = "~/result/SCcomp.root"
) {

    TFile *f_out = new TFile(output_name.c_str(),"RECREATE");

    TFile *f_in1 = new TFile(input_name1.c_str()); 
    TFile *f_in2 = new TFile(input_name2.c_str()); 

    TDirectory *dir0 = f_out->GetDirectory(Form("run%4d",runNum));
    if (!dir0) dir0 = f_out->mkdir(Form("run%4d",runNum));

    std::vector<std::vector<double>> vtxzs = make_comp_vtx(runNum,f_in1,f_in2,f_out,dir0);

    if (vtxzs.size()==0){
        return;
    }

    TH1D *hrate1 = (TH1D*)f_in1->Get("hrate")->Clone(Form("hrate%d_nsc",runNum));
    TH1D *hrate2 = (TH1D*)f_in2->Get("hrate")->Clone(Form("hrate%d_dens",runNum));

    f_out->cd();
    if (hrate1) hrate1->Write();
    if (hrate2) hrate2->Write();

    make_comp_mult(runNum,f_in1,f_in2,f_out,dir0);

    /* TH1D *hmult_full_range = (TH1D*)f_in2->Get("hmult_full_range");
    TH1D *hmultVA_full_range = (TH1D*)f_in2->Get("hmultVA_full_range");

    if (hmult_full_range){
        dir0->cd();
        hmult_full_range->Write();
        hmultVA_full_range->Write();
    } */

    std::string dname = "Diff";
    TString dtitle = Form("Run %4d, All Angles, #Delta V",runNum);

    std::vector<std::vector<double>> diff_list = make_comp_lr(runNum,f_in1,f_in2,f_out,dir0,dname,dtitle);

    
    std::string dnameh = "Diff_high";
    TString dtitleh = Form("Run %4d, 40+ Deg, #Delta V",runNum);

    std::vector<std::vector<double>> diff_high_list = make_comp_lr(runNum,f_in1,f_in2,f_out,dir0,dnameh,dtitleh);       

        

    std::string dnamel = "Diff_low";
    TString dtitlel = Form("Run %4d, 40- Deg, #Delta V",runNum);

    std::vector<std::vector<double>> diff_low_list = make_comp_lr(runNum,f_in1,f_in2,f_out,dir0,dnamel,dtitlel);


    std::string dename = "hdEdx";
    TString detitle = Form("Run %4d, Momentum",runNum);
    TDirectory *dirProj = dir0->GetDirectory("dEdx_proj");
    if (!dirProj) dirProj = dir0->mkdir("dEdx_proj");
    comp_mnt_proj(runNum,f_in1,f_in2,f_out,dirProj,dename,detitle);


    
    std::string denameVA = "hdEdxVA";
    TString detitleVA = Form("Run %4d, VA Momentum",runNum);
    TDirectory *dirProjVA = dir0->GetDirectory("dEdx_proj_VA");
    if (!dirProjVA) dirProjVA = dir0->mkdir("dEdx_proj_VA");
    comp_mnt_proj(runNum,f_in1,f_in2,f_out,dirProjVA,denameVA,detitleVA);
    

    f_out->Close();

    return;
}
