#include <iostream>
#include <fstream>
#include <iomanip>
#include <unordered_map>
#include <TCutG.h>


//cut switches; 0 -  no cut; 1 - sig cut for vertex; 2 - sig cut and mult cut

//changing how I do cuts
//do all the cuts first, and then only skip during filling histograms
//count all the events/tracks where certain cuts pass or fail


//set a static range of -40 to -20 for vtx z

//double targetZsig = 2.5;
//double targetZsig = 1.5;
double targetZsig = 2.0;
//int zsigmult=3;
int zsigmult=1;

//Cut on number of clusters for a given track
Int_t fClustCut = 15;

//Cut to check track actually originates from the vertex
Double_t fPOCACut = 20;

//adding multiplicity cut for this macro
//Int_t cutMult = 45;
Int_t cutMult = 30;
//BDC offsets
//Double_t fBDCOffsetX = -2.84168;
//Double_t fBDCOffsetY = -237.892;
    //auto fBDCOffsetX = -1.2;
    //auto fBDCOffsetY = -238.0;

//fBDCOffsetX = -4.62; // Spring Value




std::vector<TH1D*> make_mnt_list(
    std::string base_name = "hmnt",
    std::string base_title = "Momentum Distribution",
    double lwr = 0.0,
    double upr = 2000.0
){

    int n = static_cast<int>(upr-lwr);

    TH1D *h = new TH1D(Form("%s_both",base_name.c_str()),Form("%s",base_title.c_str()),n,lwr,upr);

    TH1D *hL = (TH1D*)h->Clone(Form("%s_left",base_name.c_str()));
    hL->SetTitle(Form("%s (Left Tracks)",base_title.c_str()));
    TH1D *hR = (TH1D*)h->Clone(Form("%s_right",base_name.c_str()));
    hR->SetTitle(Form("%s (Right Tracks)",base_title.c_str()));


    TH1D *hX = new TH1D(Form("%sX_both",base_name.c_str()),Form("X %s at Target Plane",base_title.c_str()),n,-upr/2,upr/2);

    TH1D *hXL = (TH1D*)h->Clone(Form("%sX_left",base_name.c_str()));
    hXL->SetTitle(Form("X %s at Target Plane (Left Tracks)",base_title.c_str()));
    TH1D *hXR = (TH1D*)h->Clone(Form("%sX_right",base_name.c_str()));
    hXR->SetTitle(Form("X %s at Target Plane (Right Tracks)",base_title.c_str()));


    TH1D *hY = new TH1D(Form("%sY_both",base_name.c_str()),Form("Y %s at Target Plane",base_title.c_str()),n,-upr/2,upr/2);

    TH1D *hYL = (TH1D*)h->Clone(Form("%sY_left",base_name.c_str()));
    hYL->SetTitle(Form("Y %s at Target Plane (Left Tracks)",base_title.c_str()));
    TH1D *hYR = (TH1D*)h->Clone(Form("%sY_right",base_name.c_str()));
    hYR->SetTitle(Form("Y %s at Target Plane (Right Tracks)",base_title.c_str()));


    TH1D *hZ = new TH1D(Form("%sZ_both",base_name.c_str()),Form("Z %s at Target Plane",base_title.c_str()),n,-upr/2,upr/2);

    TH1D *hZL = (TH1D*)h->Clone(Form("%sZ_left",base_name.c_str()));
    hZL->SetTitle(Form("Z %s at Target Plane (Left Tracks)",base_title.c_str()));
    TH1D *hZR = (TH1D*)h->Clone(Form("%sZ_right",base_name.c_str()));
    hZR->SetTitle(Form("z %s at Target Plane (Right Tracks)",base_title.c_str()));

    return {h,hL,hR,hX,hXL,hXR,hY,hYL,hYR,hZ,hZL,hZR};
}

std::vector<TH1D*> make_vtx_list(
    std::string base_name = "Vtx",
    std::string base_title = "Vertex Position",
    double lwr = 0.0,
    double upr = 2000.0
){

    int n = static_cast<int>(upr-lwr);

    TH1D *hX = new TH1D(Form("%sX",base_name.c_str()),Form("X %s",base_title.c_str()),n,-upr/2,upr/2);

    TH1D *hY = new TH1D(Form("%sY",base_name.c_str()),Form("Y %s",base_title.c_str()),n,-upr/2,upr/2);

    //TH1D *hZ = new TH1D(Form("%sZ",base_name.c_str()),Form("Z %s",base_title.c_str()),n,-upr/2,upr/2);
    TH1D *hZ = new TH1D(Form("%sZ",base_name.c_str()),Form("Z %s",base_title.c_str()),200,-40.0,-20.0);

    return {hX,hY,hZ};
}


std::vector<TH1D*> make_diff_list(
    std::string base_name = "Diff",
    std::string base_title = "Diff Dist",
    double lwr = 0.0,
    double upr = 2000.0
){

    int n = static_cast<int>(upr-lwr);

    TH1D *hX = new TH1D(Form("%sX_both",base_name.c_str()),Form("X %s",base_title.c_str()),n,-upr/2,upr/2);

    TH1D *hXL = (TH1D*)hX->Clone(Form("%sX_left",base_name.c_str()));
    hXL->SetTitle(Form("X %s (Left Tracks)",base_title.c_str()));
    TH1D *hXR = (TH1D*)hX->Clone(Form("%sX_right",base_name.c_str()));
    hXR->SetTitle(Form("X %s (Right Tracks)",base_title.c_str()));


    TH1D *hY = new TH1D(Form("%sY_both",base_name.c_str()),Form("Y %s",base_title.c_str()),n,-upr/2,upr/2);

    TH1D *hYL = (TH1D*)hY->Clone(Form("%sY_left",base_name.c_str()));
    hYL->SetTitle(Form("Y %s (Left Tracks)",base_title.c_str()));
    TH1D *hYR = (TH1D*)hY->Clone(Form("%sY_right",base_name.c_str()));
    hYR->SetTitle(Form("Y %s (Right Tracks)",base_title.c_str()));


    return {hX,hXL,hXR,hY,hYL,hYR};
}


void make_header(
    std::ofstream &mnt_out,
    int chain_index,
    UInt_t eventID,
    Int_t trackCount,
    std::string str
    
){
    mnt_out<<"Momenta of " << str<< " for event number " << chain_index<<", evtID "<<eventID<<"\n";
    mnt_out<<"Num of Tracks: " <<trackCount<<"\n";
    mnt_out<<std::left<<std::setw(10)<<"Trk Num"<<std::setw(12)<<"Mnt Mag"<<std::setw(12)<<"Tgt X Mnt "<<std::setw(12)<<"Tgt Y Mnt "<<std::setw(12)<<"Tgt Z Mnt "<<std::setw(12)<<"Tgt X Pos "<<std::setw(12)<<"Tgt Y Pos "<<std::setw(10)<<"Direction "<< "\n";
    
    return;
}


void Finder(
    int runNum = 1642,
    double fBDCOffsetX = -2.70, // Fall Value  
    double fBDCOffsetY = -181.8,// Same for Spring and Fall

    std::string inDirectory = "SC_test/data", 
    std::string pattern = "d0000",
    std::string outDirectory = "SC_test/localDens",
    int evt_num = 1000,
    int cut_switch = 1
){

    //file for rate info
    ULong64_t eventTime;
    UInt_t eventID;
    UInt_t dataID;
    ifstream metaList;
    metaList.open(TString::Format("/data/RB230064/common_2024/metadata-latest/run_%04d/metadataList.txt", runNum).Data());
    string filename;
    metaList >> filename;
    TFile *f1 = new TFile(TString::Format("/data/RB230064/common_2024/metadata-latest/run_%04d/%s", runNum, filename.c_str()).Data());

    TTree *tree_run = (TTree*)f1->Get("MetaData");
    tree_run -> SetBranchAddress("eventTime", &eventTime);
    tree_run -> SetBranchAddress("eventID", &eventID);
    tree_run -> SetBranchAddress("dataID", &dataID);
    auto total = tree_run->GetEntries();    
    TH1D *timeHist = new TH1D("timeHist", "timeHist", total / 4 , 0, total / 4);
    TH1D *timeHist_chunk = new TH1D("timeHist_chunk", "timeHist_chunk", total / 4 , 0, total / 4); //timeHist with time difference of first and last bins of each chunk
    TH1D *timeHist_avg = new TH1D("timeHist_avg","timeHist_avg", total / 4 , 0, total / 4);


    TH1D *hrate = new TH1D("hrate","Trigger Rate Distribution;Rate;Count",200,20.0,40.0);
    TH1D *hmult = new TH1D("hmult","Multiplicity Distribution;Multiplicity;Count",100,0,100);
    TH1D *hmultVA = new TH1D("hmultVA","Multiplicity Distribution, VA Tracks;Multiplicity;Count",100,0,100);

    TH1D *hmult_full_range = new TH1D("hmult_full_range","Multiplicity Distribution (Without Mult Cut);Multiplicity;Count",100,0,100);
    TH1D *hmultVA_full_range = new TH1D("hmultVA_full_range","Multiplicity Distribution, VA Tracks (Without Mult Cut);Multiplicity;Count",100,0,100);

    tree_run->GetEntry(0);
    auto initTime = eventTime;
    auto initID = eventID;
    tree_run->GetEntry(total - 1);
    auto finalTime = eventTime;
    double avg_rate = total / 4. / (finalTime - initTime) / (10.0e-9);
    cout << "Avg Rate: " << avg_rate << endl;


    TTree *t_out = new TTree("tree_output","Tree for storing event rate averages found with different methods");

    Double_t avg_rate_full_run;
    Double_t avg_rate_per_chunk;
    Double_t avg_rate_rolling;
    t_out->Branch("avg_rate_full_run",&avg_rate_full_run,"avg_rate_full_run/D"); //average rate across all events in metadata (total events/total time)
    t_out->Branch("avg_rate_per_chunk",&avg_rate_per_chunk,"avg_rate_per_chunk/D"); //average rate of each chunk (chunk events/chunk time)
    t_out->Branch("avg_rate_rolling",&avg_rate_rolling,"avg_rate_rolling/D"); //average of the "rolling average"; how I initially tried to get the average rate in each chunk

    avg_rate_full_run=avg_rate;



    vector<ULong64_t> TSbuffer;
    Int_t EventsRate = 100;

    TH1D *hrates_full = new TH1D("hrates_full", "Rates Distribution", 100, 0., 100.);


    TChain *tree = nullptr;

    TTreeReader *reader = nullptr;
    TTreeReaderValue<TClonesArray> *vertexReader = nullptr;
    TTreeReaderValue<TClonesArray> *recoReader = nullptr;
    TTreeReaderValue<TClonesArray> *vertexVAReader = nullptr;
    TTreeReaderValue<TClonesArray> *recoVAReader = nullptr;
    TTreeReaderValue<STBeamInfo> *beamInfoReader = nullptr;

    TTreeReaderValue<STEventHeader> *eventHeaderReader = nullptr;

    STBeamInfo *fBeamInfo = nullptr;

    STVertex *vertexPtr;
    STRecoTrack *recoPtr;
    
    
    STVertex *vertexVAPtr;

    //TString filePath = TString::Format("/dataspirit/teajun/reco_post_SC/sbatch/run%4d_s*.reco.bulk.root", runNum);
    //TString filePath = TString::Format("/data/RB230064/jesus/reconstruction_test/outputs/sbatch_%d/run%4d_s*.reco.bulk.root",batch_size, runNum);
    //TString filePath = TString::Format("SC_test/data/run%4d_s*_d%04d.reco.2024.root", runNum, densNum);
    TString filePath = TString::Format("%s/run%4d_s*_%s.reco.2024.root", inDirectory.c_str(),runNum, pattern.c_str());
    //TString filePath = TString::Format("%s/run%4d_s*.root", inDirectory.c_str(),runNum);

	TString log = TString::Format("%s/density%04d_%s.log", outDirectory.c_str(), runNum, pattern.c_str());

	FairLogger *logger = FairLogger::GetLogger();

    cout << filePath.Data() << endl;
    tree = new TChain("cbmsim");
    tree->Add(filePath);

    reader = new TTreeReader(tree);

    beamInfoReader = new TTreeReaderValue<STBeamInfo>(*reader, "STBeamInfo");

    eventHeaderReader = new TTreeReaderValue<STEventHeader>(*reader, "STEventHeader");

    //VAVertex and VATracks use the vertex from the BDC

    vertexVAReader = new TTreeReaderValue<TClonesArray>(*reader, "VAVertex");
    recoVAReader = new TTreeReaderValue<TClonesArray>(*reader, "VATracks");


    //STVertex and STRecoTrack use the vertex from the tracks
    vertexReader = new TTreeReaderValue<TClonesArray>(*reader, "STVertex");
    recoReader = new TTreeReaderValue<TClonesArray>(*reader, "STRecoTrack");

    int eventCount = tree->GetEntries();

    int events_per_chunk = 2000;
    int nchunks = eventCount/events_per_chunk;
    
    Double_t tbdc_x;
    Double_t tbdc_y;
    Double_t tbdc_a;
    Double_t tbdc_b;

    std::vector<TH1D*> timeHist_list_rolling_avg {}; //rolling averages split into chunks
    std::vector<TH1D*> timeHist_list_chunk_avg {}; //take the number of events in a chunk and divide by time difference of the first and last events of the chunk

    //using recoPtr
    //make sure everything is in TPC coordinates



    int nbins=100;
    int xmin=-25;
    int xmax=25;

    if (runNum < 1600){
        nbins=200;
        xmin=-50;
        xmax=50;
    }


    std::string hname = "hmnt";
    std::string htitle = "Momentum Distribution (All Angles)";

    double lwr = -2000.0;
    double upr = 2000.0;

    TH2D *hm_mVA= new TH2D("hm_mVA","Momentum VA vs Momentum (All Angles)",2000,0.0,2000.0,2000,0.0,2000.0);
    TH2D *hm_mVAL = new TH2D("hm_mVAL","Momentum VA vs Momentum (All Angles, Left Going Tracks)",2000,0.0,2000.0,2000,0.0,2000.0);
    TH2D *hm_mVAR = new TH2D("hm_mVAR","Momentum VA vs Momentum (All Angles, Right Going Tracks)",2000,0.0,2000.0,2000,0.0,2000.0);    

    TH2D *hm_mVA_high= new TH2D("hm_mVA_high","Momentum VA vs Momentum (40+ Deg)",2000,0.0,2000.0,2000,0.0,2000.0);
    TH2D *hm_mVAL_high = new TH2D("hm_mVAL_high","Momentum VA vs Momentum (40+ Deg, Left Going Tracks)",2000,0.0,2000.0,2000,0.0,2000.0);
    TH2D *hm_mVAR_high = new TH2D("hm_mVAR_high","Momentum VA vs Momentum (40+ Deg, Right Going Tracks)",2000,0.0,2000.0,2000,0.0,2000.0);    

    TH2D *hm_mVA_low= new TH2D("hm_mVA_low","Momentum VA vs Momentum (40- Deg)",2000,0.0,2000.0,2000,0.0,2000.0);
    TH2D *hm_mVAL_low = new TH2D("hm_mVAL_low","Momentum VA vs Momentum (40- Deg, Left Going Tracks)",2000,0.0,2000.0,2000,0.0,2000.0);
    TH2D *hm_mVAR_low = new TH2D("hm_mVAR_low","Momentum VA vs Momentum (40- Deg, Right Going Tracks)",2000,0.0,2000.0,2000,0.0,2000.0);    



    std::vector<TH1D*> hlist = make_mnt_list(hname,htitle,lwr,upr);
    TH2D *hdEdx = new TH2D("hdEdx","PID Plots (All Angles)",2000,0.0,2000.0,1000,0.0,1000.0);
    TH2D *hdEdxL = new TH2D("hdEdxL","PID Plots (All Angles, Left Going Tracks)",2000,0.0,2000.0,1000,0.0,1000.0);
    TH2D *hdEdxR = new TH2D("hdEdxR","PID Plots (All Angles, Right Going Tracks)",2000,0.0,2000.0,1000,0.0,1000.0);

    //left or right determined where the track thinks the vertex is (using GetMomentum.X instead of GetMomentumTargetPlane)
    TH2D *hdEdxMntL = new TH2D("hdEdxMntL","PID Plots (All Angles, Left Going Tracks, Mnt X)",2000,0.0,2000.0,1000,0.0,1000.0);
    TH2D *hdEdxMntR = new TH2D("hdEdxMntR","PID Plots (All Angles, Right Going Tracks, Mnt X)",2000,0.0,2000.0,1000,0.0,1000.0);

    TH2D *hdEdxPhi = new TH2D("hdEdxPhi","PID Plots (All Angles, Phi Cut)",2000,0.0,2000.0,1000,0.0,1000.0);
    TH2D *hdEdxPhiL = new TH2D("hdEdxPhiL","PID Plots (All Angles, Phi Cut, Left Going Tracks)",2000,0.0,2000.0,1000,0.0,1000.0);
    TH2D *hdEdxPhiR = new TH2D("hdEdxPhiR","PID Plots (All Angles, Phi Cut, Right Going Tracks)",2000,0.0,2000.0,1000,0.0,1000.0);

    TH2D *hdEdxMntPhiL = new TH2D("hdEdxMntPhiL","PID Plots (All Angles, Phi Cut, Left Going Tracks, Mnt X)",2000,0.0,2000.0,1000,0.0,1000.0);
    TH2D *hdEdxMntPhiR = new TH2D("hdEdxMntPhiR","PID Plots (All Angles, Phi Cut, Right Going Tracks, Mnt X)",2000,0.0,2000.0,1000,0.0,1000.0);



    std::string hname_VA = "hmnt_VA";
    std::string htitle_VA = "Momentum Distribution, VA Tracks (All Angles)";
    std::vector<TH1D*> hlist_VA = make_mnt_list(hname_VA,htitle_VA,lwr,upr);
    TH2D *hdEdxVA = new TH2D("hdEdxVA","PID Plots (All Angles, VA Tracks)",2000,0.0,2000.0,1000,0.0,1000.0);
    TH2D *hdEdxVAL = new TH2D("hdEdxVAL","PID Plots (All Angles, Left Going, VA Tracks)",2000,0.0,2000.0,1000,0.0,1000.0);
    TH2D *hdEdxVAR = new TH2D("hdEdxVAR","PID Plots (All Angles, Right Going, VA Tracks)",2000,0.0,2000.0,1000,0.0,1000.0);    

    TH2D *hdEdxVAMntL = new TH2D("hdEdxVAMntL","PID Plots (All Angles, Left Going, Mnt X, VA Tracks)",2000,0.0,2000.0,1000,0.0,1000.0);
    TH2D *hdEdxVAMntR = new TH2D("hdEdxVAMntR","PID Plots (All Angles, Right Going, Mnt X, VA Tracks)",2000,0.0,2000.0,1000,0.0,1000.0);    


    TH2D *hdEdxVAPhi = new TH2D("hdEdxVAPhi","PID Plots (All Angles, Phi cut, VA Tracks)",2000,0.0,2000.0,1000,0.0,1000.0);
    TH2D *hdEdxVAPhiL = new TH2D("hdEdxVAPhiL","PID Plots (All Angles, Phi cut, Left Going, VA Tracks)",2000,0.0,2000.0,1000,0.0,1000.0);
    TH2D *hdEdxVAPhiR = new TH2D("hdEdxVAPhiR","PID Plots (All Angles, Phi cut, Right Going, VA Tracks)",2000,0.0,2000.0,1000,0.0,1000.0);    

    TH2D *hdEdxVAMntPhiL = new TH2D("hdEdxVAMntPhiL","PID Plots (All Angles, Phi cut, Left Going, Mnt X, VA Tracks)",2000,0.0,2000.0,1000,0.0,1000.0);
    TH2D *hdEdxVAMntPhiR = new TH2D("hdEdxVAMntPhiR","PID Plots (All Angles, Phi cut, Right Going, Mnt X,VA Tracks)",2000,0.0,2000.0,1000,0.0,1000.0);    



    std::string vname = "Vtx";
    std::string vtitle = "Vertex Position";
    std::vector<TH1D*> vtxlist = make_vtx_list(vname,vtitle,lwr/5.0,upr/5.0);

    std::string vnameVA = "VtxVA";
    std::string vtitleVA = "Vertex Position, VA Tracks";
    std::vector<TH1D*> vtxlistVA = make_vtx_list(vnameVA,vtitleVA,lwr/5.0,upr/5.0);


    std::string vnamenm = "Vtx_no_mult";
    std::string vtitlenm = "Vertex Position";
    std::vector<TH1D*> vtxlist_no_mult = make_vtx_list(vnamenm,vtitlenm,lwr/5.0,upr/5.0);

    std::string vnameVAnm = "VtxVA_no_mult";
    std::string vtitleVAnm = "Vertex Position, VA Tracks";
    std::vector<TH1D*> vtxlistVA_no_mult = make_vtx_list(vnameVAnm,vtitleVAnm,lwr/5.0,upr/5.0);
    
    

    std::string dname = "Diff";
    std::string dtitle = "Diff Distribution (All Angles)";
    std::vector<TH1D*> difflist = make_diff_list(dname,dtitle,lwr/20.0,upr/20.0);  

    std::string hname_high = "hmnt_high";
    std::string htitle_high = "Momentum Distribution (40+ Deg)";

    std::vector<TH1D*> hlist_high = make_mnt_list(hname_high,htitle_high,lwr,upr);
    TH2D *hdEdx_high = new TH2D("hdEdx_high","PID plots (40+ Deg)",2000,0.0,2000.0,1000,0.0,1000.0);
    TH2D *hdEdxL_high = new TH2D("hdEdxL_high","PID plots (40+ Deg, Left Going Tracks)",2000,0.0,2000.0,1000,0.0,1000.0);
    TH2D *hdEdxR_high = new TH2D("hdEdxR_high","PID plots (40+ Deg, Right Going Tracks)",2000,0.0,2000.0,1000,0.0,1000.0);

    TH2D *hdEdxMntL_high = new TH2D("hdEdxMntL_high","PID Plots (40+ Deg, Left Going Tracks, Mnt X)",2000,0.0,2000.0,1000,0.0,1000.0);
    TH2D *hdEdxMntR_high = new TH2D("hdEdxMntR_high","PID Plots (40+ Deg, Right Going Tracks, Mnt X)",2000,0.0,2000.0,1000,0.0,1000.0);

    TH2D *hdEdxPhi_high = new TH2D("hdEdxPhi_high","PID plots (40+ Deg, Phi Cut)",2000,0.0,2000.0,1000,0.0,1000.0);
    TH2D *hdEdxPhiL_high = new TH2D("hdEdxPhiL_high","PID plots (40+ Deg, Phi Cut, Left Going Tracks)",2000,0.0,2000.0,1000,0.0,1000.0);
    TH2D *hdEdxPhiR_high = new TH2D("hdEdxPhiR_high","PID plots (40+ Deg, Phi Cut, Right Going Tracks)",2000,0.0,2000.0,1000,0.0,1000.0);

    TH2D *hdEdxMntPhiL_high = new TH2D("hdEdxMntPhiL_high","PID Plots (40+ Deg, Phi Cut, Left Going Tracks, Mnt X)",2000,0.0,2000.0,1000,0.0,1000.0);
    TH2D *hdEdxMntPhiR_high = new TH2D("hdEdxMntPhiR_high","PID Plots (40+ Deg, Phi Cut, Right Going Tracks, Mnt X)",2000,0.0,2000.0,1000,0.0,1000.0);


    std::string hname_VA_high = "hmnt_VA_high";
    std::string htitle_VA_high = "Momentum Distribution (40+ Deg, VA Tracks)";
    
    std::vector<TH1D*> hlist_VA_high = make_mnt_list(hname_VA_high,htitle_VA_high,lwr,upr);
    TH2D *hdEdxVA_high = new TH2D("hdEdxVA_high","PID plots (40+ Deg, VA Tracks)",2000,0.0,2000.0,1000,0.0,1000.0);
    TH2D *hdEdxVAL_high = new TH2D("hdEdxVAL_high","PID plots (40+ Deg, Left Going, VA Tracks)",2000,0.0,2000.0,1000,0.0,1000.0);
    TH2D *hdEdxVAR_high = new TH2D("hdEdxVAR_high","PID plots (40+ Deg, Right Going, VA Tracks)",2000,0.0,2000.0,1000,0.0,1000.0);    

    TH2D *hdEdxVAMntL_high = new TH2D("hdEdxVAMntL_high","PID plots (40+ Deg, Left Going, Mnt X, VA Tracks)",2000,0.0,2000.0,1000,0.0,1000.0);
    TH2D *hdEdxVAMntR_high = new TH2D("hdEdxVAMntR_high","PID plots (40+ Deg, Right Going, Mnt X, VA Tracks)",2000,0.0,2000.0,1000,0.0,1000.0);    


    TH2D *hdEdxVAPhi_high = new TH2D("hdEdxVAPhi_high","PID plots (40+ Deg, Phi Cut, VA Tracks)",2000,0.0,2000.0,1000,0.0,1000.0);
    TH2D *hdEdxVAPhiL_high = new TH2D("hdEdxVAPhiL_high","PID plots (40+ Deg, Phi Cut, Left Going, VA Tracks)",2000,0.0,2000.0,1000,0.0,1000.0);
    TH2D *hdEdxVAPhiR_high = new TH2D("hdEdxVAPhiR_high","PID plots (40+ Deg, Phi Cut, Right Going, VA Tracks)",2000,0.0,2000.0,1000,0.0,1000.0);    

    TH2D *hdEdxVAMntPhiL_high = new TH2D("hdEdxVAMntPhiL_high","PID plots (40+ Deg, Phi Cut, Left Going, Mnt X, VA Tracks)",2000,0.0,2000.0,1000,0.0,1000.0);
    TH2D *hdEdxVAMntPhiR_high = new TH2D("hdEdxVAMntPhiR_high","PID plots (40+ Deg, Phi Cut, Right Going, Mnt X, VA Tracks)",2000,0.0,2000.0,1000,0.0,1000.0);    


    std::string dname_high = "Diff_high";
    std::string dtitle_high = "Diff Distribution (40+ Deg)";
    std::vector<TH1D*> difflist_high = make_diff_list(dname_high,dtitle_high,lwr/20.0,upr/20.0);    

    
    std::string hname_low = "hmnt_low";
    std::string htitle_low = "Momentum Distribution (40- Deg)";

    std::vector<TH1D*> hlist_low = make_mnt_list(hname_low,htitle_low,lwr,upr);
    TH2D *hdEdx_low = new TH2D("hdEdx_low","PID plots (40- Deg)",2000,0.0,2000.0,1000,0.0,1000.0);
    TH2D *hdEdxL_low = new TH2D("hdEdxL_low","PID plots (40- Deg, Left Going Tracks)",2000,0.0,2000.0,1000,0.0,1000.0);
    TH2D *hdEdxR_low = new TH2D("hdEdxR_low","PID plots (40- Deg, Right Going Tracks)",2000,0.0,2000.0,1000,0.0,1000.0);

    TH2D *hdEdxMntL_low = new TH2D("hdEdxMntL_low","PID plots (40- Deg, Left Going Tracks, Mnt X)",2000,0.0,2000.0,1000,0.0,1000.0);
    TH2D *hdEdxMntR_low = new TH2D("hdEdxMntR_low","PID plots (40- Deg, Right Going Tracks, Mnt X)",2000,0.0,2000.0,1000,0.0,1000.0);


    TH2D *hdEdxPhi_low = new TH2D("hdEdxPhi_low","PID plots (40- Deg, Phi Cut)",2000,0.0,2000.0,1000,0.0,1000.0);
    TH2D *hdEdxPhiL_low = new TH2D("hdEdxPhiL_low","PID plots (40- Deg, Phi Cut, Left Going Tracks)",2000,0.0,2000.0,1000,0.0,1000.0);
    TH2D *hdEdxPhiR_low = new TH2D("hdEdxPhiR_low","PID plots (40- Deg, Phi Cut, Right Going Tracks)",2000,0.0,2000.0,1000,0.0,1000.0);


    TH2D *hdEdxMntPhiL_low = new TH2D("hdEdxMntPhiL_low","PID plots (40- Deg, Phi Cut, Left Going Tracks, Mnt X)",2000,0.0,2000.0,1000,0.0,1000.0);
    TH2D *hdEdxMntPhiR_low = new TH2D("hdEdxMntPhiR_low","PID plots (40- Deg, Phi Cut, Right Going Tracks, Mnt X)",2000,0.0,2000.0,1000,0.0,1000.0);



    std::string hname_VA_low = "hmnt_VA_low";
    std::string htitle_VA_low = "Momentum Distribution (40- Deg, VA Tracks)";
    
    std::vector<TH1D*> hlist_VA_low = make_mnt_list(hname_VA_low,htitle_VA_low,lwr,upr);
    TH2D *hdEdxVA_low = new TH2D("hdEdxVA_low","PID plots (40- Deg, VA Tracks)",2000,0.0,2000.0,1000,0.0,1000.0);
    TH2D *hdEdxVAL_low = new TH2D("hdEdxVAL_low","PID plots (40- Deg, Left Going, VA Tracks)",2000,0.0,2000.0,1000,0.0,1000.0);
    TH2D *hdEdxVAR_low = new TH2D("hdEdxVAR_low","PID plots (40- Deg, Right Going, VA Tracks)",2000,0.0,2000.0,1000,0.0,1000.0);    

    TH2D *hdEdxVAMntL_low = new TH2D("hdEdxVAMntL_low","PID plots (40- Deg, Left Going, Mnt X, VA Tracks)",2000,0.0,2000.0,1000,0.0,1000.0);
    TH2D *hdEdxVAMntR_low = new TH2D("hdEdxVAMntR_low","PID plots (40- Deg, Right Going, Mnt X, VA Tracks)",2000,0.0,2000.0,1000,0.0,1000.0);    


    TH2D *hdEdxVAPhi_low = new TH2D("hdEdxVAPhi_low","PID plots (40- Deg, Phi Cut, VA Tracks)",2000,0.0,2000.0,1000,0.0,1000.0);
    TH2D *hdEdxVAPhiL_low = new TH2D("hdEdxVAPhiL_low","PID plots (40- Deg, Phi Cut, Left Going, VA Tracks)",2000,0.0,2000.0,1000,0.0,1000.0);
    TH2D *hdEdxVAPhiR_low = new TH2D("hdEdxVAPhiR_low","PID plots (40- Deg, Phi Cut, Right Going, VA Tracks)",2000,0.0,2000.0,1000,0.0,1000.0);    

    TH2D *hdEdxVAMntPhiL_low = new TH2D("hdEdxVAMntPhiL_low","PID plots (40- Deg, Phi Cut, Left Going, Mnt X, VA Tracks)",2000,0.0,2000.0,1000,0.0,1000.0);
    TH2D *hdEdxVAMntPhiR_low = new TH2D("hdEdxVAMntPhiR_low","PID plots (40- Deg, Phi Cut, Right Going, Mnt X, VA Tracks)",2000,0.0,2000.0,1000,0.0,1000.0);    



    std::string dname_low = "Diff_low";
    std::string dtitle_low = "Diff Distribution (40- Deg)";
    std::vector<TH1D*> difflist_low = make_diff_list(dname_low,dtitle_low,lwr/20.0,upr/20.0);    



    std::vector<int> chunk_events {};
    for (int i=0;i<nchunks;i++){
        chunk_events.push_back(i*events_per_chunk);
        TH1D *timeHist_i = (TH1D*)timeHist->Clone();
        timeHist_i->SetName(Form("timeHist_rol_%d",i));
        timeHist_list_rolling_avg.push_back(timeHist_i);

        TH1D *timeHist_chk = (TH1D*)timeHist->Clone();
        timeHist_chk->SetName(Form("timeHist_chk_%d",i));
        timeHist_list_chunk_avg.push_back(timeHist_chk);

    }

    int chain_index = 0;

    tree_run->GetEntry(0);
    int entryNum = 0;
    TSbuffer.push_back(eventTime);
    int prevEventID = eventID;

    double previousTime = eventTime;
    int previousID = eventID;

    std::vector<double> avg_rate_list {};
    
    double prev_time=0.0;


    Double_t time_ch_start = 0.0;
    Double_t time_ch_stop = 0.0;

    int ch_start = 0;
    int ch_stop = 0;

    std::string mnt_txt = outDirectory + "/" + pattern + "/" + to_string(runNum) + "_mnt_check.txt";
    std::ofstream mnt_out(mnt_txt.c_str());

    mnt_out<<"Run " << runNum <<"\n";

    std::string mnt_txt_h = outDirectory + "/" + pattern + "/" + to_string(runNum) + "_mnt_check_high.txt";
    std::ofstream mnt_out_h(mnt_txt_h.c_str());

    mnt_out_h<<"Run " << runNum <<"\n";

    std::string mnt_txt_l = outDirectory + "/" + pattern + "/" + to_string(runNum) + "_mnt_check_low.txt";
    std::ofstream mnt_out_l(mnt_txt_l.c_str());

    mnt_out_l<<"Run " << runNum <<"\n";

    std::string mnt_VA_txt = outDirectory + "/" + pattern + "/" + to_string(runNum) + "_VA_mnt_check.txt";
    std::ofstream mnt_VA_out(mnt_VA_txt.c_str());

    mnt_VA_out<<"Run " << runNum <<"\n";

    std::string mnt_VA_txt_h = outDirectory + "/" + pattern + "/" + to_string(runNum) + "_VA_mnt_check_high.txt";
    std::ofstream mnt_VA_out_h(mnt_VA_txt_h.c_str());

    mnt_VA_out_h<<"Run " << runNum <<"\n";

    std::string mnt_VA_txt_l = outDirectory + "/" + pattern + "/" + to_string(runNum) + "_VA_mnt_check_low.txt";
    std::ofstream mnt_VA_out_l(mnt_txt_l.c_str());

    mnt_VA_out_l<<"Run " << runNum <<"\n";

    std::string cut_name = "no_cut";
    
    if (cut_switch==1){
        cut_name = "vtx_cut";
    } else if (cut_switch>1){
        cut_name = "mult_cut"; //multiplicity cut is supposed to be applied after the vertex sigma cut, and should only be on if vertex cut is already on
    }



    std::string evt_log = outDirectory + "/" + pattern + "/" + to_string(runNum) + "evt_log" + cut_name + ".txt";
    std::ofstream evt_log_out(evt_log.c_str());

    evt_log_out<<"Run " << runNum <<"\n";

    int evt_count = 0; //all events in chain
    int ep_vtx = 0; //evts that pass vtx z sig cut
    int ef_vtx = 0; //evts that fail vtx z cut
    int ep_vtxVA = 0; //evts that pass vtxVA z sig cut
    int ef_vtxVA = 0; //evts that fail vtxVA z cut
    int ep_mult = 0; //evts that pass multiplicity cut
    int ef_mult = 0; //evts that fail mult cut
    int ep_multVA = 0; //evts that pass multiplicity cut (VA Tracks)
    int ef_multVA = 0; //evts that fail mult cut (VA Tracks)

    int nTracks_tot = 0; //total num tracks (Reco)
    int nTr_clust_f = 0; //num tracks that fail cluster cut
    int nTr_POCA_f = 0; //num tracks that fail POCA cut


    int nTracksVA_tot = 0; //total num tracks (VA)
    int nTrVA_clust_f = 0; //num VA tracks that fail cluster cut
    int nTrVA_POCA_f = 0; //num VA tracks that fail POCA cut
    int nTracksVA_p = 0; //num VA tracks that pass all cuts    

    int nTracks_dEdx = 0; //num tracks that fill dEdx
    int nTracks_dEdxL = 0; //num tracks that fill dEdxL
    int nTracks_dEdxR = 0; //num tracks that fill dEdxR

    int nTracks_dEdxVA = 0; //num tracks that fill dEdxVA
    int nTracks_dEdxVAL = 0; //num tracks that fill dEdxAVL
    int nTracks_dEdxVAR = 0; //num tracks that fill dEdxVAR


    //build vertex z pos to use 
    double vertexZCenter;
    double vertexZCenterVA;


    if (cut_switch > 0) {
        TH1D hVertexZPeak("hVertexZPeak", "Vertex Z Peak (Reco)", 200, -40.0, -20.0);
        TH1D hVertexZPeakVA("hVertexZPeakVA", "Vertex Z Peak (VA)", 200, -40.0, -20.0);

        for (int entry = 0; entry < eventCount; entry++) {
            reader->SetEntry(entry);

            fBeamInfo = beamInfoReader->Get();
            if (fBeamInfo == NULL) {
                continue;
            }
            if (fBeamInfo->fXTargetPlane == 0 && fBeamInfo->fYTargetPlane == 0) {
                continue;
            }

            auto vtxReco = dynamic_cast<STVertex *>((*vertexReader)->At(0));
            if (vtxReco != NULL) {
                hVertexZPeak.Fill(vtxReco->GetPos().Z());
            }

            auto vtxVA = dynamic_cast<STVertex *>((*vertexVAReader)->At(0));
            if (vtxVA != NULL) {
                hVertexZPeakVA.Fill(vtxVA->GetPos().Z());
            }
        }

        if (hVertexZPeak.GetEntries() > 0) {
            vertexZCenter = hVertexZPeak.GetXaxis()->GetBinCenter(hVertexZPeak.GetMaximumBin());
        }
        if (hVertexZPeakVA.GetEntries() > 0) {
            vertexZCenterVA = hVertexZPeakVA.GetXaxis()->GetBinCenter(hVertexZPeakVA.GetMaximumBin());
        }

        std::cout << "Reco vertex-Z center (peak): " << vertexZCenter << std::endl;
        std::cout << "VA vertex-Z center (peak): " << vertexZCenterVA << std::endl;
    }




    for (int ti=1;ti<total;ti++){
        if (chain_index>=eventCount){
            break;
        }
        auto it = std::upper_bound(chunk_events.begin(),chunk_events.end(),chain_index);
        int k = std::distance(chunk_events.begin(),it)-1;       
        Double_t time_start;
        Double_t time_stop; 

        tree_run->GetEntry(ti-1);
        previousTime = eventTime;
        previousID = eventID;
        tree_run->GetEntry(ti);
        reader->SetEntry(chain_index);
        auto eventHeader = eventHeaderReader->Get();
        auto evtID = eventHeader->GetEventID();


        if (evtID != eventID){
            continue;
        }
        
        if (chain_index==k*events_per_chunk){ 
            time_ch_start=eventTime*10.0e-9;
            std::cout<<"time_chunk_start: "<<time_ch_start<<"\n";
        }
        if (chain_index==(k+1)*events_per_chunk-1){
            time_ch_stop=eventTime*10.0e-9;
            std::cout<<"time_chunk_stop: "<<time_ch_stop<<"\n";
            avg_rate_per_chunk=events_per_chunk/(time_ch_stop-time_ch_start);
            for (int i=k*events_per_chunk+1;i<=(k+1)*events_per_chunk;i++){ //chain_index = entry bin number - 1 
                timeHist_chunk->SetBinContent(i, avg_rate_per_chunk);
                timeHist_list_chunk_avg[k]->SetBinContent(i, avg_rate_per_chunk);
            }        
            avg_rate_list.push_back(avg_rate_per_chunk);

        }
        
        
        if(TSbuffer.size() < EventsRate)
            TSbuffer.push_back(eventTime);
        else
            TSbuffer[chain_index % EventsRate] = eventTime;
        
        double rate = 0.0;
        if(chain_index > EventsRate) {
            Double_t timeDiff = (TSbuffer[chain_index % EventsRate] - TSbuffer[(chain_index+1) % EventsRate]) * (10.0e-9);
            rate = EventsRate / timeDiff;
            timeHist->SetBinContent(chain_index+1, rate);
            timeHist_list_rolling_avg[k]->SetBinContent(chain_index+1, rate);
            hrates_full->Fill(rate);
        }

        timeHist_avg->SetBinContent(chain_index,avg_rate);


        chain_index++;

        evt_count++;

        auto trackCount = recoReader->Get()->GetEntries();   //trackCount should be greater than or equal to trackCountVA
        auto trackCountVA = recoVAReader->Get()->GetEntries();       

        nTracks_tot += static_cast<int>(trackCount);
        nTracksVA_tot += static_cast<int>(trackCountVA);

        vertexPtr = dynamic_cast<STVertex *>((*vertexReader)->At(0));
        fBeamInfo = beamInfoReader->Get();

        tbdc_x = fBeamInfo->fXTargetPlane; //where the beam hits the target
        tbdc_y = fBeamInfo->fYTargetPlane; 

        if(fBeamInfo->fXTargetPlane == 0 && fBeamInfo->fYTargetPlane == 0)
            continue;

        bool vtx_check = true;
        bool success = true;
        bool successVA = true;

        if(vertexPtr == NULL) {
            vtx_check = false;
            success = false;
            if (cut_switch>0){
                ef_vtx++;
            }

        }
        TVector3 vertex;
        if (vtx_check){
            vertex = vertexPtr->GetPos();
        } 


        if (vtx_check && cut_switch > 0){
            if((vertex.Z() > vertexZCenter + zsigmult * targetZsig || vertex.Z() < vertexZCenter - zsigmult * targetZsig)){
                success = false;
                ef_vtx++;
            } else {
                success = true;
                ep_vtx++;
            }
        }


        vertexVAPtr = dynamic_cast<STVertex *>((*vertexVAReader)->At(0));

        bool vtxVA_check = true;
        if (vertexVAPtr == NULL){
            vtxVA_check = false;
            successVA = false;
            if (cut_switch>0){
                ef_vtxVA++;
            }
        }
        TVector3 vertexVA;
        if (vtxVA_check){
            vertexVA = vertexVAPtr->GetPos();
        }
        

        if (vtxVA_check && cut_switch > 0){
            if(vertexVA.Z() > vertexZCenterVA + zsigmult * targetZsig || vertexVA.Z() < vertexZCenterVA - zsigmult * targetZsig){
                successVA = false;
                ef_vtxVA++;
            } else {
                successVA = true;
                ep_vtxVA++;
            }
        }



        int multiplicity = 0;

        for(int r = 0; r < trackCount; r++) {
            recoPtr = dynamic_cast<STRecoTrack*>((*recoReader)->At(r));

            if(recoPtr == NULL) {
                cout << "recoPtr is null" << endl;
                continue;
            }
            if(vtx_check && (recoPtr->GetPOCAVertex() - vertex).Mag() <= fPOCACut) {    
                multiplicity++;
            }
        }

        if (cut_switch>1){ //>1 is obviously >0, so multiplicity cut should be applied only if vertex cut is already applied
            if (success){
                hmult_full_range->Fill(multiplicity);
                vtxlist_no_mult[0]->Fill(vertex.X()); //vertex X pos, all angles
                vtxlist_no_mult[1]->Fill(vertex.Y()); //vertex Y pos, all angles
                vtxlist_no_mult[2]->Fill(vertex.Z()); //vertex A pos, all angles
            }

            if (multiplicity<cutMult){
                success = false;
                ef_mult++;
            } else {
                //success = true;
                if (success){ //only add to event count if BOTH cuts succeed 
                    ep_mult++;
                }
            }
        }


        //build a map from reco to VA
        //if reco fails or doesn't exits, make VA also fail


        int multiplicityVA = 0;

        std::unordered_map<int, STRecoTrack *> vaTrackByRecoID;

        for(int r = 0; r < trackCountVA; r++) {
            auto recoVAPtr = dynamic_cast<STRecoTrack *>((*recoVAReader)->At(r));
            if(recoVAPtr == NULL) {
                cout << "recoVAPtr is null" << endl;
                continue;
            }

            auto recoID = recoVAPtr->GetRecoID();
            if(recoID  < 0 || recoID >= recoVAReader->Get()->GetEntries()){
                continue;
            }

            vaTrackByRecoID[recoID] = recoVAPtr;

            if(vtxVA_check && (recoVAPtr->GetPOCAVertex() - vertexVA).Mag() <= fPOCACut) {
                multiplicityVA++;
            } 
        }

        if (!success){
            successVA=false;
        }

        if (cut_switch>1){
            if (successVA){
                hmultVA_full_range->Fill(multiplicityVA);
                vtxlistVA_no_mult[0]->Fill(vertexVA.X()); //vertex X pos, all angles
                vtxlistVA_no_mult[1]->Fill(vertexVA.Y()); //vertex Y pos, all angles
                vtxlistVA_no_mult[2]->Fill(vertexVA.Z()); //vertex A pos, all angles
            }

            if (multiplicityVA<cutMult){
                successVA = false;
                ef_multVA++;
            } else {
                if (successVA){
                    ep_multVA++;
                }
            }

            if (success){

            }
        }



        if (success){
            vtxlist[0]->Fill(vertex.X()); //vertex X pos, all angles
            vtxlist[1]->Fill(vertex.Y()); //vertex Y pos, all angles
            vtxlist[2]->Fill(vertex.Z()); //vertex A pos, all angles
            hmult->Fill(multiplicity);
            if (rate!=0.0){
                hrate->Fill(rate); //filling only if reco tracks succeed
            }
        }
        if (successVA){
            vtxlistVA[0]->Fill(vertexVA.X()); //vertex X pos, all angles
            vtxlistVA[1]->Fill(vertexVA.Y()); //vertex Y pos, all angles
            vtxlistVA[2]->Fill(vertexVA.Z()); //vertex A pos, all angles  
            hmultVA->Fill(multiplicityVA);
        }        
        
        if (chain_index <= evt_num){
            std::string ap = "all particles";
            make_header(mnt_out,chain_index,eventID,trackCount,ap);
            make_header(mnt_out_h,chain_index,eventID,trackCount,ap);
            make_header(mnt_out_l,chain_index,eventID,trackCount,ap);

            make_header(mnt_VA_out,chain_index,eventID,trackCount,ap);
            make_header(mnt_VA_out_h,chain_index,eventID,trackCount,ap);
            make_header(mnt_VA_out_l,chain_index,eventID,trackCount,ap);


        }

        bool suc_track = true;
        for(int r = 0; r < trackCount; r++) {
            recoPtr = dynamic_cast<STRecoTrack *>((*recoReader)->At(r));
            if(recoPtr == NULL) {
                cout << "recoPtr is null" << endl;
                continue;
            }
            auto recoID = recoPtr->GetRecoID();
            if(recoID  < 0 || recoID >= recoReader->Get()->GetEntries()){
                continue;
            }

            if(recoPtr->GetNumLayerClusters() + recoPtr->GetNumRowClusters() <= fClustCut){
                nTr_clust_f++;
            }

            if(!vtx_check || (recoPtr->GetPOCAVertex() - vertex).Mag() > fPOCACut){
                nTr_POCA_f++;
            }
                
            if(vtx_check && recoPtr->GetNumLayerClusters() + recoPtr->GetNumRowClusters() > fClustCut && (recoPtr->GetPOCAVertex() - vertex).Mag() <= fPOCACut && success) { //should now be looking at only successful events/tracks

                auto mom = recoPtr->GetMomentum().Mag(); //from hits
                auto momTgt = recoPtr->GetMomentumTargetPlane(); 
                auto dedx = recoPtr->GetdEdxWithCut(0, 0.7, 0.5);

                auto momX = recoPtr->GetMomentum().X();

                //auto charge = recoPtr->GetCharge();
                //auto gfCharge = recoPtr->GetGenfitCharge();

                auto pos = recoPtr->GetPosTargetPlane();
                auto ang = atan(abs(momTgt.X()) / momTgt.Z()) / 3.1415926 * 180;

                auto momPhi =  TMath::RadToDeg() * momTgt.Phi();
                bool phi_check = false;
                if ((momPhi> -45 && momPhi <25) || (momPhi >150 && momPhi <180) || (momPhi >-180 && momPhi <-150)){
                    phi_check = true;
                }            
                
                std::string p_dir;
                if (momTgt.X()>0){
                    p_dir="Left";
                } else if (momTgt.X()<0){
                    p_dir="Right";
                }

                hlist[0]->Fill(mom); //mnt both dir, all angles
                hlist[3]->Fill(momTgt.X()); //x mnt at target both dir, all angles
                hlist[6]->Fill(momTgt.Y()); //y mnt at target both dir, all angles
                hlist[9]->Fill(momTgt.Z()); //z mnt at target both dir, all angles           
                hdEdx->Fill(mom,dedx); //PID both dir, all angles
                nTracks_dEdx++;

                if (phi_check){
                    hdEdxPhi->Fill(mom,dedx);
                }
                
                //auto recoVAPtr = dynamic_cast<STRecoTrack *>((*recoVAReader)->At(r)); //my logic
                STRecoTrack *recoVAPtr = NULL; //Jesus's logic
                auto vaMatch = vaTrackByRecoID.find(recoID);
                if (vaMatch != vaTrackByRecoID.end()) {
                    recoVAPtr = vaMatch->second;
                }

                
                double momVA_copy = 0.0;
                double momVATgtX_copy = 0.0;
                if(recoVAPtr != NULL) {
                    auto momVA = recoVAPtr->GetMomentum().Mag();
                    auto momVATgt = recoVAPtr->GetMomentumTargetPlane(); 
                    hm_mVA->Fill(mom,momVA);
                    momVA_copy = static_cast<double>(momVA);
                    momVATgtX_copy = static_cast<double>(momVATgt.X());
                }

                difflist[0]->Fill(-(pos.X() - (tbdc_x + fBDCOffsetX))); //diff X both dir, all angles
                difflist[3]->Fill(-(pos.Y() - (tbdc_y + fBDCOffsetY))); //diff Y both dir, all angles

                if (chain_index<=evt_num){
                    mnt_out<<std::left<<std::setw(10)<< r <<std::setw(12)<< mom <<std::setw(12)<<momTgt.X()<<std::setw(12)<<momTgt.Y()<<std::setw(12)<<momTgt.Z()<<std::setw(12)<<pos.X()<<std::setw(12)<<pos.Y()<<std::setw(10)<<p_dir<< "\n";
                }


                if (momTgt.X()>0){
                    hlist[1]->Fill(mom); //mnt left, all angles
                    hlist[4]->Fill(momTgt.X()); //x mnt at target, left, all angles
                    hlist[7]->Fill(momTgt.Y()); //y mnt at target, left, all angles
                    hlist[10]->Fill(momTgt.Z()); //z mnt at target, left, all angles 
                    hdEdxL->Fill(mom,dedx); //PID left, all angles
                    nTracks_dEdxL++;
                    if (recoVAPtr != NULL){
                        hm_mVAL->Fill(mom,momVA_copy);
                    } 
                    if (phi_check){
                        hdEdxPhiL->Fill(mom,dedx);
                    }

                    difflist[1]->Fill(-(pos.X() - (tbdc_x + fBDCOffsetX))); //diff X left, all angles
                    difflist[4]->Fill(-(pos.Y() - (tbdc_y + fBDCOffsetY))); //diff Y left, all angles                
                    
                } else if (momTgt.X()<0){
                    hlist[2]->Fill(mom); //mnt right, all angles
                    hlist[5]->Fill(momTgt.X()); //x mnt at target, right, all angles
                    hlist[8]->Fill(momTgt.Y()); //y mnt at target, right, all angles
                    hlist[11]->Fill(momTgt.Z()); //z mnt at target, right, all angles    
                    hdEdxR->Fill(mom,dedx); //PID right, all angles
                    nTracks_dEdxR++;
                    if (recoVAPtr != NULL){
                        hm_mVAR->Fill(mom,momVA_copy);                 
                    } 
                    if (phi_check){
                        hdEdxPhiR->Fill(mom,dedx);
                    }

                    difflist[2]->Fill(-(pos.X() - (tbdc_x + fBDCOffsetX))); //diff X right, all angles
                    difflist[5]->Fill(-(pos.Y() - (tbdc_y + fBDCOffsetY))); //diff Y right, all angles                    
               
                }

                if (momX>0.0){
                    hdEdxMntL->Fill(mom,dedx);
                    if (phi_check){
                        hdEdxMntPhiL->Fill(mom,dedx);
                    }
                    if (ang>40.0){
                        hdEdxMntL_high->Fill(mom,dedx);
                        if (phi_check){
                            hdEdxMntPhiL_high->Fill(mom,dedx);
                        }
                    } else if (ang<40.0){
                        hdEdxMntL_low->Fill(mom,dedx);
                        if (phi_check){
                            hdEdxMntPhiL_low->Fill(mom,dedx);
                        }
                    }
                } else if (momX<0.0){
                    hdEdxMntR->Fill(mom,dedx);
                    if (phi_check){
                        hdEdxMntPhiR->Fill(mom,dedx);
                    }
                    if (ang>40.0){
                        hdEdxMntR_high->Fill(mom,dedx);
                        if (phi_check){
                            hdEdxMntPhiR_high->Fill(mom,dedx);
                        }
                    } else if (ang<40.0){
                        hdEdxMntR_low->Fill(mom,dedx);
                        if (phi_check){
                            hdEdxMntPhiR_low->Fill(mom,dedx);
                        }
                    }
                }


                if (ang>40.0){
                    hlist_high[0]->Fill(mom); //mnt both dir, higher angles
                    hlist_high[3]->Fill(momTgt.X()); //x mnt at target both dir, higher angles
                    hlist_high[6]->Fill(momTgt.Y()); //y mnt at target both dir, higher angles
                    hlist_high[9]->Fill(momTgt.Z()); //z mnt at target both dir, higher angles           
                    hdEdx_high->Fill(mom,dedx); //PID both dir, all angles
                    if(recoVAPtr != NULL) {
                        hm_mVA_high->Fill(mom,momVA_copy);
                    }

                    if (phi_check){
                        hdEdxPhi_high->Fill(mom,dedx);
                    }


                    difflist_high[0]->Fill(-(pos.X() - (tbdc_x + fBDCOffsetX))); //diff X both dir, higher angles
                    difflist_high[3]->Fill(-(pos.Y() - (tbdc_y + fBDCOffsetY))); //diff Y both dir, higher angles

                    if (chain_index<=evt_num){
                        mnt_out_h<<std::left<<std::setw(10)<< r <<std::setw(12)<< mom <<std::setw(12)<<momTgt.X()<<std::setw(12)<<momTgt.Y()<<std::setw(12)<<momTgt.Z()<<std::setw(10)<<std::setw(12)<<pos.X()<<std::setw(12)<<pos.Y()<<p_dir<< "\n";
                    }

                    if (momTgt.X()>0){
                        hlist_high[1]->Fill(mom); //mnt left, all angles
                        hlist_high[4]->Fill(momTgt.X()); //x mnt at target, left, higher angles
                        hlist_high[7]->Fill(momTgt.Y()); //y mnt at target, left, higher angles
                        hlist_high[10]->Fill(momTgt.Z()); //z mnt at target, left, higher angles  
                        hdEdxL_high->Fill(mom,dedx); //PID left, higher angles
                        if (recoVAPtr != NULL){
                            hm_mVAL_high->Fill(mom,momVA_copy);
                        }
            
                        if (phi_check){
                            hdEdxPhiL_high->Fill(mom,dedx);
                        }
                        
                        difflist_high[1]->Fill(-(pos.X() - (tbdc_x + fBDCOffsetX))); //diff X left, higher angles
                        difflist_high[4]->Fill(-(pos.Y() - (tbdc_y + fBDCOffsetY))); //diff Y left, higher angles                                           
                    } else if (momTgt.X()<0){
                        hlist_high[2]->Fill(mom); //mnt right, higher angles
                        hlist_high[5]->Fill(momTgt.X()); //x mnt at target, right, higher angles
                        hlist_high[8]->Fill(momTgt.Y()); //y mnt at target, right, higher angles
                        hlist_high[11]->Fill(momTgt.Z()); //z mnt at target, right, higher angles 
                        hdEdxR_high->Fill(mom,dedx); //PID right, higher angles
                        if (recoVAPtr != NULL){
                            hm_mVAR_high->Fill(mom,momVA_copy);
                        }
                        if (phi_check){
                            hdEdxPhiR_high->Fill(mom,dedx);
                        }
                        
                        difflist_high[2]->Fill(-(pos.X() - (tbdc_x + fBDCOffsetX))); //diff X right, higher angles
                        difflist_high[5]->Fill(-(pos.Y() - (tbdc_y + fBDCOffsetY))); //diff Y right, higher angles  
                    }
                } else if (ang<40.0){
                    hlist_low[0]->Fill(mom); //mnt both dir, lower angles
                    hlist_low[3]->Fill(momTgt.X()); //x mnt at target both dir, lower angles
                    hlist_low[6]->Fill(momTgt.Y()); //y mnt at target both dir, lower angles
                    hlist_low[9]->Fill(momTgt.Z()); //z mnt at target both dir, lower angles           
                    hdEdx_low->Fill(mom,dedx); //PID both dir, all angles
                    if (recoVAPtr != NULL){
                        hm_mVA_low->Fill(mom,momVA_copy);
                    }
            
                    if (phi_check){
                        hdEdxPhi_low->Fill(mom,dedx);
                    }
                        


                    difflist_low[0]->Fill(-(pos.X() - (tbdc_x + fBDCOffsetX))); //diff X both dir, lower angles
                    difflist_low[3]->Fill(-(pos.Y() - (tbdc_y + fBDCOffsetY))); //diff Y both dir, lower angles


                    if (chain_index<=evt_num){
                        mnt_out_l<<std::left<<std::setw(10)<< r <<std::setw(12)<< mom <<std::setw(12)<<momTgt.X()<<std::setw(12)<<momTgt.Y()<<std::setw(12)<<momTgt.Z()<<std::setw(12)<<pos.X()<<std::setw(12)<<pos.Y()<<std::setw(10)<<p_dir<< "\n";
                    }
                    if (momTgt.X()>0){
                        hlist_low[1]->Fill(mom); //mnt left, all angles
                        hlist_low[4]->Fill(momTgt.X()); //x mnt at target, left, lower angles
                        hlist_low[7]->Fill(momTgt.Y()); //y mnt at target, left, lower angles
                        hlist_low[10]->Fill(momTgt.Z()); //z mnt at target, left, lower angles           
                        hdEdxL_low->Fill(mom,dedx); //PID left, lower angles  
                        if (recoVAPtr != NULL){   
                            hm_mVAL_low->Fill(mom,momVA_copy);
                        }                      
            
                        if (phi_check){
                            hdEdxPhiL_low->Fill(mom,dedx);
                        }
                        
                        difflist_low[1]->Fill(-(pos.X() - (tbdc_x + fBDCOffsetX))); //diff X left, lower angles
                        difflist_low[4]->Fill(-(pos.Y() - (tbdc_y + fBDCOffsetY))); //diff Y left, lower angles
                                                      
                    } else if (momTgt.X()<0){
                        hlist_low[2]->Fill(mom); //mnt right, all angles
                        hlist_low[5]->Fill(momTgt.X()); //x mnt at target, right, lower angles
                        hlist_low[8]->Fill(momTgt.Y()); //y mnt at target, right, lower angles
                        hlist_low[11]->Fill(momTgt.Z()); //z mnt at target, right, lower angles  
                        hdEdxR_low->Fill(mom,dedx); //PID right, lower angles
                        if (recoVAPtr != NULL){
                            hm_mVAR_low->Fill(mom,momVA_copy);
                        }

                        if (phi_check){
                            hdEdxPhiR_low->Fill(mom,dedx);
                        }
                        
                        difflist_low[2]->Fill(-(pos.X() - (tbdc_x + fBDCOffsetX))); //diff X right, lower angles
                        difflist_low[5]->Fill(-(pos.Y() - (tbdc_y + fBDCOffsetY))); //diff Y right, lower angles                       

                    }                    
                }
            }
        }
        for(int r = 0; r < trackCountVA; r++) {
            auto recoVAPtr = dynamic_cast<STRecoTrack *>((*recoVAReader)->At(r));
            if(recoVAPtr == NULL) {
                cout << "recoVAPtr is null" << endl;
                continue;
            }
            auto recoID = recoVAPtr->GetRecoID();
            if(recoID  < 0 || recoID >= recoVAReader->Get()->GetEntries()){
                continue;
            }
            if(recoVAPtr->GetNumLayerClusters() + recoVAPtr->GetNumRowClusters() <= fClustCut){
                nTrVA_clust_f++;
            }
            if (!vtxVA_check || (recoVAPtr->GetPOCAVertex() - vertexVA).Mag() > fPOCACut) {
                nTrVA_POCA_f++;
            }
            if(vtxVA_check && recoVAPtr->GetNumLayerClusters() + recoVAPtr->GetNumRowClusters() > fClustCut && (recoVAPtr->GetPOCAVertex() - vertexVA).Mag() <= fPOCACut && successVA) {
                
                auto momVA = recoVAPtr->GetMomentum().Mag(); //add vertex info
                auto momVATgt = recoVAPtr->GetMomentumTargetPlane(); 
                auto dedxVA = recoVAPtr->GetdEdxWithCut(0, 0.7, 0.5);

                auto momXVA = recoVAPtr->GetMomentum().X();

                auto posVA = recoVAPtr->GetPosTargetPlane();
                auto angVA = atan(abs(momVATgt.X()) / momVATgt.Z()) / 3.1415926 * 180;

                auto momVAPhi =  TMath::RadToDeg() * momVATgt.Phi();
                bool VAphi_check = false;
                if ((momVAPhi> -45 && momVAPhi <25) || (momVAPhi >150 && momVAPhi <180) || (momVAPhi >-180 && momVAPhi <-150)){
                    VAphi_check = true;
                }                

                std::string p_dir_VA;
                if (momVATgt.X()>0){
                    p_dir_VA="Left";
                } else if (momVATgt.X()<0){
                    p_dir_VA="Right";
                }

                hlist_VA[0]->Fill(momVA); //mntVA  both dir, all angles
                hlist_VA[3]->Fill(momVATgt.X()); //x mntVA at target both dir, all angles
                hlist_VA[6]->Fill(momVATgt.Y()); //y mntVA at target both dir, all angles
                hlist_VA[9]->Fill(momVATgt.Z()); //z mntVA at target both dir, all angles           
                hdEdxVA->Fill(momVA,dedxVA); //PID both dir, all angles
                nTracks_dEdxVA++;
                
                if (VAphi_check){
                    hdEdxVAPhi->Fill(momVA,dedxVA);
                }            

                difflist[0]->Fill(-(posVA.X() - (tbdc_x + fBDCOffsetX))); //diff X both dir, all angles
                difflist[3]->Fill(-(posVA.Y() - (tbdc_y + fBDCOffsetY))); //diff Y both dir, all angles

                if (chain_index<=evt_num){
                    mnt_VA_out<<std::left<<std::setw(10)<< r <<std::setw(12)<< momVA <<std::setw(12)<<momVATgt.X()<<std::setw(12)<<momVATgt.Y()<<std::setw(12)<<momVATgt.Z()<<std::setw(12)<<posVA.X()<<std::setw(12)<<posVA.Y()<<std::setw(10)<<p_dir_VA<< "\n";
                }

                if (momVATgt.X()>0){
                    hlist_VA[1]->Fill(momVA); //mntVA left, all angles
                    hlist_VA[4]->Fill(momVATgt.X()); //x mntVA at target, left, all angles
                    hlist_VA[7]->Fill(momVATgt.Y()); //y mntVA at target, left, all angles
                    hlist_VA[10]->Fill(momVATgt.Z()); //z mntVA at target, left, all angles 
                    hdEdxVAL->Fill(momVA,dedxVA); //PID left, all angles   
                    nTracks_dEdxVAL++;
                    if (VAphi_check){
                        hdEdxVAPhiL->Fill(momVA,dedxVA);
                    }
                } else if (momVATgt.X()<0){
                    hlist_VA[2]->Fill(momVA); //mntVA right, all angles
                    hlist_VA[5]->Fill(momVATgt.X()); //x mntVA at target, right, all angles
                    hlist_VA[8]->Fill(momVATgt.Y()); //y mntVA at target, right, all angles
                    hlist_VA[11]->Fill(momVATgt.Z()); //z mntVA at target, right, all angles    
                    hdEdxVAR->Fill(momVA,dedxVA); //PID right, all angles  
                    nTracks_dEdxVAR++;
                    if (VAphi_check){
                        hdEdxVAPhiR->Fill(momVA,dedxVA);
                    }
                }

                if (momXVA>0.0){
                    hdEdxVAMntL->Fill(momVA,dedxVA);
                    if (VAphi_check){
                        hdEdxVAMntPhiL->Fill(momVA,dedxVA);
                    }
                    if (angVA>40.0){
                        hdEdxVAMntL_high->Fill(momVA,dedxVA);
                        if (VAphi_check){
                            hdEdxVAMntPhiL_high->Fill(momVA,dedxVA);
                        }
                    } else if (angVA<40.0){
                        hdEdxVAMntL_low->Fill(momVA,dedxVA);
                        if (VAphi_check){
                            hdEdxVAMntPhiL_low->Fill(momVA,dedxVA);
                        }
                    }
                } else if (momXVA<0.0){
                    hdEdxVAMntR->Fill(momVA,dedxVA);
                    if (VAphi_check){
                        hdEdxVAMntPhiR->Fill(momVA,dedxVA);
                    }
                    if (angVA>40.0){
                        hdEdxVAMntR_high->Fill(momVA,dedxVA);
                        if (VAphi_check){
                            hdEdxVAMntPhiR_high->Fill(momVA,dedxVA);
                        }
                    } else if (angVA<40.0){
                        hdEdxVAMntR_low->Fill(momVA,dedxVA);
                        if (VAphi_check){
                            hdEdxVAMntPhiR_low->Fill(momVA,dedxVA);
                        }
                    }
                }                
                
                if (angVA>40.0){
                    hlist_VA_high[0]->Fill(momVA); //mntVA both dir, higher angles
                    hlist_VA_high[3]->Fill(momVATgt.X()); //x mntVA at target both dir, higher angles
                    hlist_VA_high[6]->Fill(momVATgt.Y()); //y mntVA at target both dir, higher angles
                    hlist_VA_high[9]->Fill(momVATgt.Z()); //z mntVA at target both dir, higher angles           
                    hdEdxVA_high->Fill(momVA,dedxVA); //PID both dir, all angles

                    if (VAphi_check){
                        hdEdxVAPhi_high->Fill(momVA,dedxVA);
                    }
                        
                    if (chain_index<=evt_num){
                        mnt_VA_out_h<<std::left<<std::setw(10)<< r <<std::setw(12)<< momVA <<std::setw(12)<<momVATgt.X()<<std::setw(12)<<momVATgt.Y()<<std::setw(12)<<momVATgt.Z()<<std::setw(10)<<std::setw(12)<<posVA.X()<<std::setw(12)<<posVA.Y()<<p_dir_VA<< "\n";
                    }

                    if (momVATgt.X()>0){
                        hlist_VA_high[1]->Fill(momVA); //mntVA left, all angles
                        hlist_VA_high[4]->Fill(momVATgt.X()); //x mntVA at target, left, higher angles
                        hlist_VA_high[7]->Fill(momVATgt.Y()); //y mntVA at target, left, higher angles
                        hlist_VA_high[10]->Fill(momVATgt.Z()); //z mntVA at target, left, higher angles  
                        hdEdxVAL_high->Fill(momVA,dedxVA); //PID left, higher angles                      
                        if (VAphi_check){
                            hdEdxVAPhiL_high->Fill(momVA,dedxVA);
                        }                        
                       
                    } else if (momVATgt.X()<0){
                        hlist_VA_high[2]->Fill(momVA); //mntVA right, higher angles
                        hlist_VA_high[5]->Fill(momVATgt.X()); //x mntVA at target, right, higher angles
                        hlist_VA_high[8]->Fill(momVATgt.Y()); //y mntVA at target, right, higher angles
                        hlist_VA_high[11]->Fill(momVATgt.Z()); //z mntVA at target, right, higher angles 
                        hdEdxVAR_high->Fill(momVA,dedxVA); //PID right, higher angles    
                        if (VAphi_check){
                            hdEdxVAPhiR_high->Fill(momVA,dedxVA);
                        }                    
                    }
                } else if (angVA<40.0){
                    hlist_VA_low[0]->Fill(momVA); //mntVA both dir, lower angles
                    hlist_VA_low[3]->Fill(momVATgt.X()); //x mntVA at target both dir, lower angles
                    hlist_VA_low[6]->Fill(momVATgt.Y()); //y mntVA at target both dir, lower angles
                    hlist_VA_low[9]->Fill(momVATgt.Z()); //z mntVA at target both dir, lower angles           
                    hdEdxVA_low->Fill(momVA,dedxVA); //PID both dir, all angles


                    if (VAphi_check){
                        hdEdxVAPhi_low->Fill(momVA,dedxVA);
                    }
                    if (chain_index<=evt_num){
                        mnt_VA_out_l<<std::left<<std::setw(10)<< r <<std::setw(12)<< momVA <<std::setw(12)<<momVATgt.X()<<std::setw(12)<<momVATgt.Y()<<std::setw(12)<<momVATgt.Z()<<std::setw(12)<<posVA.X()<<std::setw(12)<<posVA.Y()<<std::setw(10)<<p_dir_VA<< "\n";
                    }

                    if (momVATgt.X()>0){
                        hlist_VA_low[1]->Fill(momVA); //mntVA left, all angles
                        hlist_VA_low[4]->Fill(momVATgt.X()); //x mntVA at target, left, lower angles
                        hlist_VA_low[7]->Fill(momVATgt.Y()); //y mntVA at target, left, lower angles
                        hlist_VA_low[10]->Fill(momVATgt.Z()); //z mntVA at target, left, lower angles           
                        hdEdxVAL_low->Fill(momVA,dedxVA); //PID left, lower angles                             
    
                        if (VAphi_check){
                            hdEdxVAPhiL_low->Fill(momVA,dedxVA);
                        }                        
                             
                    } else if (momVATgt.X()<0){
                        hlist_VA_low[2]->Fill(momVA); //mntVA right, all angles
                        hlist_VA_low[5]->Fill(momVATgt.X()); //x mntVA at target, right, lower angles
                        hlist_VA_low[8]->Fill(momVATgt.Y()); //y mntVA at target, right, lower angles
                        hlist_VA_low[11]->Fill(momVATgt.Z()); //z mntVA at target, right, lower angles  
                        hdEdxVAR_low->Fill(momVA,dedxVA); //PID right, lower angles                     
                        if (VAphi_check){
                            hdEdxVAPhiR_low->Fill(momVA,dedxVA);
                        }
                    }                    
                }
            } 
        }


    }

    evt_log_out<<"Total Number of Events: "<<evt_count<<"\n";
    if (cut_switch>0){
        evt_log_out<<"Events that Pass Vtx Z Sig Cut: "<<ep_vtx<<"\n";
        evt_log_out<<"Events that Fail Vtx Z Sig Cut: "<<ef_vtx<<"\n";

        evt_log_out<<"Events that Pass VAVtx Z Sig Cut: "<<ep_vtxVA<<"\n";
        evt_log_out<<"Events that Fail VAVtx Z Sig Cut: "<<ef_vtxVA<<"\n";  
        evt_log_out<<"\n";      
    }

    if (cut_switch>1){
        evt_log_out<<"Multiplicity Cut applied only if Vtx Z Sig Cut is already applied"<<"\n";
        evt_log_out<<"Events that Pass Multiplicity Cut (Reco Tracks): "<<ep_mult<<"\n";
        evt_log_out<<"Events that Fail Multiplicity Cut (Reco Tracks): "<<ef_mult<<"\n";
        evt_log_out<<"Events that Pass Multiplicity Cut (VA Tracks): "<<ep_multVA<<"\n";
        evt_log_out<<"Events that Fail Multiplicity Cut (VA Tracks): "<<ef_multVA<<"\n";   
        evt_log_out<<"\n";     
    }


    evt_log_out<<"Total Number of Reco Tracks: "<<nTracks_tot<<"\n";
    evt_log_out<<"Number of Reco Tracks that Fail Cluster Cut: "<<nTr_clust_f<<"\n";
    evt_log_out<<"Number of Reco Tracks that Fail POCA Cut: "<<nTr_POCA_f<<"\n";
    evt_log_out<<"Total Number of Reco Tracks that fill dEdx: " << nTracks_dEdx<<"\n";
    evt_log_out<<"Total Number of Reco Tracks that fill dEdxL: " << nTracks_dEdxL<<"\n";
    evt_log_out<<"Total Number of Reco Tracks that fill dEdxR: " << nTracks_dEdxR<<"\n";
    evt_log_out<<"\n"; 
    
    evt_log_out<<"Total Number of VA Tracks: "<<nTracksVA_tot<<"\n";
    evt_log_out<<"Number of VA Tracks that Fail Cluster Cut: "<<nTrVA_clust_f<<"\n";
    evt_log_out<<"Number of VA Tracks that Fail POCA Cut: "<<nTrVA_POCA_f<<"\n";
    evt_log_out<<"Total Number of VA Tracks that fill dEdxVA: " << nTracks_dEdxVA<<"\n";
    evt_log_out<<"Total Number of VA Tracks that fill dEdxVAL: " << nTracks_dEdxVAL<<"\n";
    evt_log_out<<"Total Number of VA Tracks that fill dEdxVAR: " << nTracks_dEdxVAR<<"\n";


    TString outFilePath = TString::Format("%s/density%4d_%s.root",outDirectory.c_str(), runNum, pattern.c_str());
    
    TFile *outFile =  new TFile(outFilePath.Data(), "RECREATE");

    outFile->cd();
    timeHist->Write();
    timeHist_chunk->Write();
    timeHist_avg->Write();

    TCanvas *ctime = new TCanvas("ctime");
    timeHist->SetLineColor(kBlue);
    timeHist_chunk->SetLineColor(kRed);
    timeHist_avg->SetLineColor(kGreen);
    timeHist->Draw("SAME");
    timeHist_chunk->Draw("SAME");
    timeHist_avg->Draw("SAME");

    ctime->Write();

    hrates_full->Write();
    hrate->Write();
    hmult->Write();
    hmultVA->Write();

    if (cut_switch>1){
        hmult_full_range->Write();
        hmultVA_full_range->Write();        
    }

    hdEdx->Write();
    hdEdxL->Write();
    hdEdxR->Write();

    hdEdxMntL->Write();
    hdEdxMntR->Write();

    hdEdxPhi->Write();
    hdEdxPhiL->Write();
    hdEdxPhiR->Write();

    hdEdxMntPhiL->Write();
    hdEdxMntPhiR->Write();


    hdEdxVA->Write();
    hdEdxVAL->Write();
    hdEdxVAR->Write();

    hdEdxVAMntL->Write();
    hdEdxVAMntR->Write();

    hdEdxVAPhi->Write();
    hdEdxVAPhiL->Write();
    hdEdxVAPhiR->Write();

    hdEdxVAMntPhiL->Write();
    hdEdxVAMntPhiR->Write();

    
    
    hm_mVA->Write();
    hm_mVAL->Write();
    hm_mVAR->Write();

    for (size_t i=0;i<hlist.size();i++){
        hlist[i]->Write();
    }

    for (size_t i=0;i<hlist_VA.size();i++){
        hlist_VA[i]->Write();
    }   

    for (size_t i=0;i<vtxlist.size();i++){
        vtxlist[i]->Write();
    }

    for (size_t i=0;i<vtxlistVA.size();i++){
        vtxlistVA[i]->Write();
    }

    if (cut_switch>1){
        for (size_t i=0;i<vtxlist_no_mult.size();i++){
            vtxlist_no_mult[i]->Write();
        }

        for (size_t i=0;i<vtxlistVA_no_mult.size();i++){
            vtxlistVA_no_mult[i]->Write();
        }

    }

    for (size_t i=0;i<difflist.size();i++){
        difflist[i]->Write();
    }

    hdEdx_high->Write();
    hdEdxL_high->Write();    
    hdEdxR_high->Write();

    hdEdxMntL_high->Write();
    hdEdxMntR_high->Write();

    hdEdxPhi_high->Write();
    hdEdxPhiL_high->Write();
    hdEdxPhiR_high->Write();

    hdEdxMntPhiL_high->Write();
    hdEdxMntPhiR_high->Write();    

    hdEdxVA_high->Write();
    hdEdxVAL_high->Write();    
    hdEdxVAR_high->Write();
    
    hdEdxVAMntL_high->Write();    
    hdEdxVAMntR_high->Write();

    hdEdxVAPhi_high->Write();
    hdEdxVAPhiL_high->Write();
    hdEdxVAPhiR_high->Write();

    hdEdxVAMntPhiL_high->Write();
    hdEdxVAMntPhiR_high->Write();    
    
    hm_mVA_high->Write();
    hm_mVAL_high->Write();
    hm_mVAR_high->Write();

    for (size_t i=0;i<hlist_high.size();i++){
        hlist_high[i]->Write();
    }

    for (size_t i=0;i<hlist_VA_high.size();i++){
        hlist_VA_high[i]->Write();
    }



    for (size_t i=0;i<difflist_high.size();i++){
        difflist_high[i]->Write();
    }


    hdEdx_low->Write();
    hdEdxL_low->Write();    
    hdEdxR_low->Write();    

    hdEdxMntL_low->Write();    
    hdEdxMntR_low->Write();

    hdEdxPhi_low->Write();
    hdEdxPhiL_low->Write();    
    hdEdxPhiR_low->Write(); 

    hdEdxMntPhiL_low->Write();    
    hdEdxMntPhiR_low->Write(); 

    hdEdxVA_low->Write();
    hdEdxVAL_low->Write();    
    hdEdxVAR_low->Write();   
    
    hdEdxVAMntL_low->Write();    
    hdEdxVAMntR_low->Write(); 

    hdEdxVAPhi_low->Write();
    hdEdxVAPhiL_low->Write();    
    hdEdxVAPhiR_low->Write(); 

    hdEdxVAMntPhiL_low->Write();    
    hdEdxVAMntPhiR_low->Write(); 


    hm_mVA_low->Write();
    hm_mVAL_low->Write();
    hm_mVAR_low->Write();

    for (size_t i=0;i<hlist_low.size();i++){
        hlist_low[i]->Write();
    }

    for (size_t i=0;i<hlist_VA_low.size();i++){
        hlist_VA_low[i]->Write();
    }

    for (size_t i=0;i<difflist_low.size();i++){
        difflist_low[i]->Write();
    }


    evt_log_out.close();
    mnt_out.close();
    mnt_out_h.close();
    mnt_out_l.close();

    mnt_VA_out.close();
    mnt_VA_out_h.close();
    mnt_VA_out_l.close();

    outFile->Close();

}


void SCfinder3(
    int runNum = 1234,
    double BDCOffsetX = -2.70,
    double BDCOffsetY = -181.80,    
    std::string inDirectory = "SC_test/data", 
    std::string pattern = "d0000",
    std::string outDirectory = "SC_test/localDens",
    int evt_num = 1000, //event number I want to look at to compare momenta
    int cut_switch = 1
){
    Finder(runNum,BDCOffsetX,BDCOffsetY,inDirectory,pattern,outDirectory,evt_num,cut_switch);
	gApplication->Terminate();
}
