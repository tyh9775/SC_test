#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <algorithm>
#include <TSpectrum.h>
#include <TFile.h>
#include <TDirectory.h>
#include <TGraphErrors.h>


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
    delete c;
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

    delete c;
    delete c1;
    delete c2;
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

    delete c;
    delete c1;
    delete c2;
    return;
}


std::vector<double> make_comp_vtx(
    int runNum,
    TFile *f_in1,
    TFile *f_out,
    TDirectory *parentDir
){
    TH1D *hvx1 = (TH1D*)f_in1->Get("VtxX")->Clone(Form("VtxX_run%4d",runNum));
    TH1D *hvy1 = (TH1D*)f_in1->Get("VtxY")->Clone(Form("VtxY_run%4d",runNum));
    TH1D *hvz1 = (TH1D*)f_in1->Get("VtxZ")->Clone(Form("VtxZ_run%4d",runNum));

    if (hvz1==nullptr || hvz1->GetEntries()==0){
        return {};
    }

    TH1D *hvxva1 = (TH1D*)f_in1->Get("VtxVAX")->Clone(Form("VtxVAX_run%4d",runNum));
    TH1D *hvyva1 = (TH1D*)f_in1->Get("VtxVAY")->Clone(Form("VtxVAY_run%4d",runNum));
    TH1D *hvzva1 = (TH1D*)f_in1->Get("VtxVAZ")->Clone(Form("VtxVAZ_run%4d",runNum));


    TDirectory *dirVtxX = parentDir->GetDirectory("VtxX");
    TDirectory *dirVtxY = parentDir->GetDirectory("VtxY");
    TDirectory *dirVtxZ = parentDir->GetDirectory("VtxZ");

    if (!dirVtxX) dirVtxX = parentDir->mkdir("VtxX");
    if (!dirVtxY) dirVtxY = parentDir->mkdir("VtxY");
    if (!dirVtxZ) dirVtxZ = parentDir->mkdir("VtxZ");

    TF1 *fit1 = new TF1("fit1", "gaus", -1000, 1000);
    TString ctx1 = "Vtx X Reco and VA Track Comparison;Vtx (mm);Counts";
    TCanvas *cvx1 = new TCanvas(Form("VtxX_run%4d_comp",runNum));
    write_canvas_2_alt(cvx1,hvx1,hvxva1,ctx1,f_out,dirVtxX,fit1);
        
    TString cty1 = "Vtx Y Reco and VA Track Comparison;Vtx (mm);Counts";
    TCanvas *cvy1 = new TCanvas(Form("VtxY_run%4d_comp",runNum));
    write_canvas_2_alt(cvy1,hvy1,hvyva1,cty1,f_out,dirVtxY,fit1);
    
    TString ctz1 = "Vtx Z Reco and VA Track Comparison ;Vtx (mm);Counts";
    TCanvas *cvz1 = new TCanvas(Form("VtxZ_run%4d_comp",runNum));
    std::vector<double> vz1 = write_canvas_2_alt(cvz1,hvz1,hvzva1,ctz1,f_out,dirVtxZ,fit1);

    return vz1;
}


void make_comp_mult(
    int runNum,
    TFile *f_in1,
    TFile *f_out,
    TDirectory *parentDir
){
    TH1D *hmult = (TH1D*)f_in1->Get("hmult")->Clone(Form("hmult%4d",runNum));
    TH1D *hmultVA = (TH1D*)f_in1->Get("hmultVA")->Clone(Form("hmultVA%4d",runNum));

    if (hmult==nullptr || hmult->GetEntries()==0){
        return;
    }

    TDirectory *dirMult = parentDir->GetDirectory("mult");
    if (!dirMult) dirMult = parentDir->mkdir("mult");

    TString ct1 = "Multiplicity From Reco and VA Tracks Comparison;Multiplicity;Counts";
    TCanvas *c1 = new TCanvas(Form("Multi_%4d_comp",runNum));
    write_canvas_mult(c1,hmult,hmultVA,ct1,f_out,dirMult);

    return;
}



std::vector<std::vector<double>> make_comp_lr(
    int runNum,
    TFile *f_in1,
    TFile *f_out,
    TDirectory *parentDir,
    std::string hname,
    TString ctitle
){
    TH1D *hdxl = (TH1D*)f_in1->Get(Form("%sX_left",hname.c_str()))->Clone(Form("%sX_left_run%4d",hname.c_str(),runNum));
    TH1D *hdxr = (TH1D*)f_in1->Get(Form("%sX_right",hname.c_str()))->Clone(Form("%sX_right_run%4d",hname.c_str(),runNum));

    TH1D *hdyl = (TH1D*)f_in1->Get(Form("%sY_left",hname.c_str()))->Clone(Form("%sY_left_run%4d",hname.c_str(),runNum));
    TH1D *hdyr = (TH1D*)f_in1->Get(Form("%sY_right",hname.c_str()))->Clone(Form("%sY_right_run%4d",hname.c_str(),runNum));

    TF1 *fit1 = new TF1("fit1", "gaus", -100, 100);


    TDirectory *dirDXLR = parentDir->GetDirectory("DXLR");
    TDirectory *dirDYLR = parentDir->GetDirectory("DYLR");


    if (!dirDXLR) dirDXLR = parentDir->mkdir("DXLR");
    if (!dirDYLR) dirDYLR = parentDir->mkdir("DYLR");


    TString ctx = Form("%s_{X} Left and Right Comparison;#Delta V_{X} (mm);Counts",ctitle.Data());
    TCanvas *cdx = new TCanvas(Form("%sX_run%4d",hname.c_str(),runNum));
    write_canvas_2(cdx,hdxl,hdxr,ctx,f_out,dirDXLR,fit1);

    TString cty = Form("%s_{Y} Left and Right Comparison;#Delta V_{y} (mm);Counts",ctitle.Data());
    TCanvas *cdy = new TCanvas(Form("%sY_run%4d",hname.c_str(),runNum));
    write_canvas_2(cdy,hdyl,hdyr,cty,f_out,dirDYLR,fit1);

    std::vector<double> dxlr = diff_find(hdxl,hdxr);
    std::vector<double> dylr = diff_find(hdyl,hdyr);


    return {dxlr,dylr};
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

    c->Update();
    c->Write();
    delete c;

    return;
}


void comp_mnt_proj(
    int runNum,
    TFile *f_in1,
    TFile *f_out,
    TDirectory *parentDir,
    std::string hname,
    TString ctitle,
    std::vector<double> dedx_list = {40.0,80.0,120.0,160.0,200.0,240.0,280.0,320.0,360.0,400.0}
){

    TH2D *hl = (TH2D*)f_in1->Get(Form("%sL",hname.c_str()))->Clone(Form("%sL",hname.c_str()));
    hl->RebinX(2);
    TH2D *hr = (TH2D*)f_in1->Get(Form("%sR",hname.c_str()))->Clone(Form("%sR",hname.c_str()));;
    hr->RebinX(2);

    TH2D *hlm = (TH2D*)f_in1->Get(Form("%sMntL",hname.c_str()))->Clone(Form("%sMntL",hname.c_str()));
    hlm->RebinX(2);
    TH2D *hrm = (TH2D*)f_in1->Get(Form("%sMntR",hname.c_str()))->Clone(Form("%sMntR",hname.c_str()));
    hrm->RebinX(2);
    
    TH2D *hlp = (TH2D*)f_in1->Get(Form("%sPhiL",hname.c_str()))->Clone(Form("%sPhiL",hname.c_str()));
    hlp->RebinX(2);
    TH2D *hrp = (TH2D*)f_in1->Get(Form("%sPhiR",hname.c_str()))->Clone(Form("%sPhiR",hname.c_str()));
    hrp->RebinX(2);
    
    TH2D *hlmp = (TH2D*)f_in1->Get(Form("%sMntPhiL",hname.c_str()))->Clone(Form("%sMntPhiL",hname.c_str()));
    hlmp->RebinX(2);
    TH2D *hrmp = (TH2D*)f_in1->Get(Form("%sMntPhiR",hname.c_str()))->Clone(Form("%sMntPhiR",hname.c_str()));
    hrmp->RebinX(2);

    TH2D *hlh = (TH2D*)f_in1->Get(Form("%sL_high",hname.c_str()))->Clone(Form("%sL_high",hname.c_str()));
    hlh->RebinX(2);
    TH2D *hrh = (TH2D*)f_in1->Get(Form("%sR_high",hname.c_str()))->Clone(Form("%sR_high",hname.c_str()));
    hrh->RebinX(2);


    TH2D *hlmh = (TH2D*)f_in1->Get(Form("%sMntL_high",hname.c_str()))->Clone(Form("%sMntL_high",hname.c_str()));
    hlmh->RebinX(2);
    TH2D *hrmh = (TH2D*)f_in1->Get(Form("%sMntR_high",hname.c_str()))->Clone(Form("%sMntR_high",hname.c_str()));  
    hrmh->RebinX(2);

    TH2D *hlph = (TH2D*)f_in1->Get(Form("%sPhiL_high",hname.c_str()))->Clone(Form("%sPhiL_high",hname.c_str()));
    hlph->RebinX(2);
    TH2D *hrph = (TH2D*)f_in1->Get(Form("%sPhiR_high",hname.c_str()))->Clone(Form("%sPhiR_high",hname.c_str()));
    hrph->RebinX(2);
    
    TH2D *hlmph = (TH2D*)f_in1->Get(Form("%sMntPhiL_high",hname.c_str()))->Clone(Form("%sMntPhiL_high",hname.c_str()));
    hlmph->RebinX(2);
    TH2D *hrmph = (TH2D*)f_in1->Get(Form("%sMntPhiR_high",hname.c_str()))->Clone(Form("%sMntPhiR_high",hname.c_str()));
    hrmph->RebinX(2);


    TH2D *hllw = (TH2D*)f_in1->Get(Form("%sL_low",hname.c_str()))->Clone(Form("%sL_low",hname.c_str()));
    hllw->RebinX(2);
    TH2D *hrlw = (TH2D*)f_in1->Get(Form("%sR_low",hname.c_str()))->Clone(Form("%sR_low",hname.c_str()));
    hrlw->RebinX(2);

    TH2D *hlmlw = (TH2D*)f_in1->Get(Form("%sMntL_low",hname.c_str()))->Clone(Form("%sMntL_low",hname.c_str()));
    hlmlw->RebinX(2);
    TH2D *hrmlw = (TH2D*)f_in1->Get(Form("%sMntR_low",hname.c_str()))->Clone(Form("%sMntR_low",hname.c_str()));
    hrmlw->RebinX(2);

    TH2D *hlplw = (TH2D*)f_in1->Get(Form("%sPhiL_low",hname.c_str()))->Clone(Form("%sPhiL_low",hname.c_str()));
    hlplw->RebinX(2);
    TH2D *hrplw = (TH2D*)f_in1->Get(Form("%sPhiR_low",hname.c_str()))->Clone(Form("%sPhiR_low",hname.c_str()));
    hrplw->RebinX(2);
    
    TH2D *hlmplw = (TH2D*)f_in1->Get(Form("%sMntPhiL_low",hname.c_str()))->Clone(Form("%sMntPhiL_low",hname.c_str()));
    hlmplw->RebinX(2);
    TH2D *hrmplw = (TH2D*)f_in1->Get(Form("%sMntPhiR_low",hname.c_str()))->Clone(Form("%sMntPhiR_low",hname.c_str()));
    hrmplw->RebinX(2);


    TDirectory *dirLR = parentDir->GetDirectory("LR");
    TDirectory *dirMntLR = parentDir->GetDirectory("MntLR");
    TDirectory *dirPhiLR = parentDir->GetDirectory("PhiLR");
    TDirectory *dirMntPhiLR = parentDir->GetDirectory("MntPhiLR");


    if (!dirLR) dirLR = parentDir->mkdir("LR");
    if (!dirMntLR) dirMntLR = parentDir->mkdir("MntLR");
    if (!dirPhiLR) dirPhiLR = parentDir->mkdir("PhiLR");
    if (!dirMntPhiLR) dirMntPhiLR = parentDir->mkdir("MntPhiLR");



    for (size_t i=0;i<dedx_list.size();i++){
        f_out->cd();
        int dedx=static_cast<int>(dedx_list[i]);

        TString ctlr = Form("%s Left and Right Comparison (All Angles);Momentum (MeV/c);Counts",ctitle.Data());
        TCanvas *clr = new TCanvas(Form("%s_projX_%d_run%4d_comp_lr",hname.c_str(),dedx,runNum));
        write_canvas_proj(dedx_list[i],clr,hl,hr,ctlr,f_out,dirLR,"left","right");

        TF1 *f1 = new TF1("f1","gaus",0.0,2000.0);
        TString ctlrh = Form("%s Left and Right Comparison (40+ Deg);Momentum (MeV/c);Counts",ctitle.Data());
        TCanvas *clrh = new TCanvas(Form("%s_projX_%d_run%4d_comp_lr_high",hname.c_str(),dedx,runNum));
        write_canvas_proj(dedx_list[i],clrh,hlh,hrh,ctlrh,f_out,dirLR,"left","right");

        TString ctlrl = Form("%s Left and Right Comparison(40- Deg);Momentum (MeV/c);Counts",ctitle.Data());
        TCanvas *clrl = new TCanvas(Form("%s_projX_%d_run%4d_comp_lr_low",hname.c_str(),dedx,runNum));
        write_canvas_proj(dedx_list[i],clrl,hllw,hrlw,ctlrl,f_out,dirLR,"left","right");



        TString ctlrm = Form("%s Left and Right Comparison(All Angles, Mnt X);Momentum (MeV/c);Counts",ctitle.Data());
        TCanvas *clrm = new TCanvas(Form("%s_projX_%d_run%4d_comp_lr_mnt",hname.c_str(),dedx,runNum));
        write_canvas_proj(dedx_list[i],clrm,hlm,hrm,ctlrm,f_out,dirMntLR,"left","right");

        TString ctlrmh = Form("%s Left and Right Comparison (40+ Deg, Mnt X);Momentum (MeV/c);Counts",ctitle.Data());
        TCanvas *clrmh = new TCanvas(Form("%s_projX_%d_run%4d_comp_lr_mnt_high",hname.c_str(),dedx,runNum));
        write_canvas_proj(dedx_list[i],clrmh,hlmh,hrmh,ctlrmh,f_out,dirMntLR,"left","right");

        TString ctlrml = Form("%s Left and Right Comparison (40- Deg, Mnt X);Momentum (MeV/c);Counts",ctitle.Data());
        TCanvas *clrml = new TCanvas(Form("%s_projX_%d_run%4d_comp_lr_mnt_low",hname.c_str(),dedx,runNum));
        write_canvas_proj(dedx_list[i],clrml,hlmlw,hrmlw,ctlrml,f_out,dirMntLR,"left","right");


        TString ctlrp = Form("%s Left and Right Comparison (All Angles, Phi Cut);Momentum (MeV/c);Counts",ctitle.Data());
        TCanvas *clrp = new TCanvas(Form("%s_projX_%d_run%4d_comp_lr_phi",hname.c_str(),dedx,runNum));
        write_canvas_proj(dedx_list[i],clrp,hlp,hrp,ctlrp,f_out,dirPhiLR,"left","right");

        TString ctlrph = Form("%s Left and Right Comparison (40+ Deg, Phi Cut);Momentum (MeV/c);Counts",ctitle.Data());
        TCanvas *clrph = new TCanvas(Form("%s_projX_%d_run%4d_comp_lr_phi_high",hname.c_str(),dedx,runNum));
        write_canvas_proj(dedx_list[i],clrph,hlph,hrph,ctlrph,f_out,dirPhiLR,"left","right");

        TString ctlrpl = Form("%s Left and Right Comparison (40- Deg, Phi Cut);Momentum (MeV/c);Counts",ctitle.Data());
        TCanvas *clrpl = new TCanvas(Form("%s_projX_%d_run%4d_comp_lr_phi_low",hname.c_str(),dedx,runNum));
        write_canvas_proj(dedx_list[i],clrpl,hlplw,hrplw,ctlrpl,f_out,dirPhiLR,"left","right");        


        TString ctlrmp = Form("%s Left and Right Comparison (All Angles, Mnt X, Phi Cut);Momentum (MeV/c);Counts",ctitle.Data());
        TCanvas *clrmp = new TCanvas(Form("%s_projX_%d_run%4d_comp_lr_mnt_phi",hname.c_str(),dedx,runNum));
        write_canvas_proj(dedx_list[i],clrmp,hlmp,hrmp,ctlrmp,f_out,dirMntPhiLR,"left","right");

        TString ctlrmph = Form("%s Left and Right Comparison (40+ Deg, Mnt X, Phi Cut);Momentum (MeV/c);Counts",ctitle.Data());
        TCanvas *clrmph = new TCanvas(Form("%s_projX_%d_run%4d_comp_lr_mnt_phi_high",hname.c_str(),dedx,runNum));
        write_canvas_proj(dedx_list[i],clrmph,hlmph,hrmph,ctlrmph,f_out,dirMntPhiLR,"left","right");

        TString ctlrmpl = Form("%s Left and Right Comparison (40- Deg, Mnt X, Phi Cut);Momentum (MeV/c);Counts",ctitle.Data());
        TCanvas *clrmpl = new TCanvas(Form("%s_projX_%d_run%4d_comp_lr_mnt_phi_low",hname.c_str(),dedx,runNum));
        write_canvas_proj(dedx_list[i],clrmpl,hlmplw,hrmplw,ctlrmpl,f_out,dirMntPhiLR,"left","right");


    }


    return;
}





void SCcombine_sd(
    std::vector<int> runs = {1642},
    std::string dens="single_dens",
    std::string input_name1 = "~/dens",
    std::string output_name = "~/result/SCcomp_fall.root"
) {
    
    gROOT->SetBatch(kTRUE);
    
    TFile *f_out = new TFile(output_name.c_str(),"RECREATE");



        
    for (size_t i=0;i<runs.size();i++){
        int runNum = runs[i];
        std::cout<<runNum<<"\n";


        int color_index = i+1;
        if (color_index==5){
            color_index=46;
        }
        if (color_index==3){
            color_index=30;
        }
        if (color_index==10){
            color_index=49;
        }

        TDirectory *dir0 = f_out->GetDirectory(Form("run%4d",runNum));
        if (!dir0) dir0 = f_out->mkdir(Form("run%4d",runNum));


        std::vector<double> dx_run {};
        std::vector<double> dx_run_err {};

        std::vector<double> dx_run_h {};
        std::vector<double> dx_run_err_h {};
        
        std::vector<double> dx_run_l {};
        std::vector<double> dx_run_err_l {};

        std::vector<double> dy_run {};
        std::vector<double> dy_run_err {};

        std::vector<double> dy_run_h {};
        std::vector<double> dy_run_err_h {};
        
        std::vector<double> dy_run_l {};
        std::vector<double> dy_run_err_l {};  
        

        TFile *f_in1 = new TFile(Form("%s/density%d_%s.root",input_name1.c_str(),runNum,dens.c_str()));
        TH1D *hrate = (TH1D*)f_in1->Get("hrate")->Clone(Form("hrate%d",runNum));
        f_out->cd();
        if (hrate) hrate->Write();

        TH1D *timeHist_avg = (TH1D*)f_in1->Get("timeHist_avg");
                
                        
        std::vector<double> vtxzs = make_comp_vtx(runNum,f_in1,f_out,dir0);

        make_comp_mult(runNum,f_in1,f_out,dir0);


        std::string dname = "Diff";
        TString dtitle = Form("Run %4d, All Angles, #Delta V", runNum);

        std::vector<std::vector<double>> diff_list = make_comp_lr(runNum,f_in1,f_out,dir0,dname,dtitle);

        dx_run.push_back(diff_list[0][0]);
        dx_run_err.push_back(diff_list[0][1]);
        dy_run.push_back(diff_list[1][0]);
        dy_run_err.push_back(diff_list[1][1]);

        std::string dnameh = "Diff_high";
        TString dtitleh = Form("Run %4d, 40+ Deg, #Delta V", runNum);

        std::vector<std::vector<double>> diff_high_list = make_comp_lr(runNum,f_in1,f_out,dir0,dnameh,dtitleh);       

        dx_run_h.push_back(diff_high_list[0][0]);
        dx_run_err_h.push_back(diff_high_list[0][1]);
        dy_run_h.push_back(diff_high_list[1][0]);
        dy_run_err_h.push_back(diff_high_list[1][1]);

        std::string dnamel = "Diff_low";
        TString dtitlel = Form("Run %4d, 40- Deg, #Delta V", runNum);

        std::vector<std::vector<double>> diff_low_list = make_comp_lr(runNum,f_in1,f_out,dir0,dnamel,dtitlel);

        dx_run_l.push_back(diff_low_list[0][0]);
        dx_run_err_l.push_back(diff_low_list[0][1]);
        dy_run_l.push_back(diff_low_list[1][0]);
        dy_run_err_l.push_back(diff_low_list[1][1]);


        std::string dename = "hdEdx";
        TString detitle = Form("Run %4d, Momentum",runNum);
        TDirectory *dirProj = dir0->GetDirectory("dEdx_proj");
        if (!dirProj) dirProj = dir0->mkdir("dEdx_proj");
        comp_mnt_proj(runNum,f_in1,f_out,dirProj,dename,detitle);

        
        std::string denameVA = "hdEdxVA";
        TString detitleVA = Form("Run %4d, VA Momentum",runNum);
        TDirectory *dirProjVA = dir0->GetDirectory("dEdx_proj_VA");
        if (!dirProjVA) dirProjVA = dir0->mkdir("dEdx_proj_VA");
        comp_mnt_proj(runNum,f_in1,f_out,dirProjVA,denameVA,detitleVA);

        f_in1->Close();

        
    }


    f_out->Close();

    return;
}
