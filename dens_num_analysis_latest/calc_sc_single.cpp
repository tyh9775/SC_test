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




void calc_sc_single(
    int runNum = 1642,
    std::string input_name1 = "dens_no_dens.root",
    std::string output_name = "~/result/SCcomp.root"
) {

    TFile *f_out = new TFile(output_name.c_str(),"RECREATE");

    TFile *f_in1 = new TFile(input_name1.c_str()); 

    TDirectory *dir0 = f_out->GetDirectory(Form("run%4d",runNum));
    if (!dir0) dir0 = f_out->mkdir(Form("run%4d",runNum));



    std::string dname = "Diff";
    TString dtitle = Form("Run %4d, All Angles, #Delta V",runNum);

    std::vector<std::vector<double>> diff_list = make_comp_lr(runNum,f_in1,f_in2,f_out,dir0,dname,dtitle);

    
    std::string dnameh = "Diff_high";
    TString dtitleh = Form("Run %4d, 40+ Deg, #Delta V",runNum);

    std::vector<std::vector<double>> diff_high_list = make_comp_lr(runNum,f_in1,f_in2,f_out,dir0,dnameh,dtitleh);       

        

    std::string dnamel = "Diff_low";
    TString dtitlel = Form("Run %4d, 40- Deg, #Delta V",runNum);

    std::vector<std::vector<double>> diff_low_list = make_comp_lr(runNum,f_in1,f_in2,f_out,dir0,dnamel,dtitlel);

    

    f_out->Close();

    return;
}
