#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

TString getVersion(const std::string &pth);

//original version treats indiviual runs as if they all have the same average event rate
//new version lets event rates vary
//bin each range of event rates (width 1Hz for now), and see what the charge densities are at those points

void run_reco_SC_par(
	Int_t fRunNo = 1634,
    Int_t fSplitNo = 0,
    Int_t fNumEventsInSplit = 1000,
    TString fOutForm = "reco.2024",
    //TString fPathToData = "data",
	TString fPathToData = "/data/RB230064/teajun/SC_test/data/",

	double fFieldOffsetX = -0.1794, //old values
  	double fFieldOffsetY = -20.5502,
  	double fFieldOffsetZ = 58.0526,

	double p0 = 0.0,
	double p1 = 0.0,

	double fBDCOffsetX = -2.70, //old values
	double fBDCOffsetY = -181.8,
	
	double fTargetZ = -30.82,
	
	Bool_t fUseComputedYDisplacementInMap = kFALSE,
  	Bool_t fApplyYCorrectionToClusters = kFALSE,

    Bool_t fUseMeta = kTRUE,
    Bool_t fIsFRIBDAQ = true
) {
	cout << "running the macro" << endl;
	TString fSpiRITROOTPath = TString(gSystem->Getenv("VMCWORKDIR")) + "/";
    TString gainCalibFile=fSpiRITROOTPath + "parameters/gainCalibration_groundPlane_2024spring_threshold_10.root";
    double constant = 8.065E-2;
    double linear = 2.352E-3;
    double quadratic = -4.789E-8;


    double fDriftVelocity = 5.45;
    double fGGtb = 33;


	cout<< fUseComputedYDisplacementInMap<<"\n";
	cout<< fApplyYCorrectionToClusters<<"\n";
    TString fSupplePath = "/data/RB230064/common_2024/metadata-latest";

    ifstream dvFile;
    if(fRunNo < 1600)
        dvFile.open("/data/RB230064/teajun/driftVelocitySummary124Xe.txt");
    else
        dvFile.open("/data/RB230064/teajun/driftVelocitySummary136Xe.txt");

    if(!dvFile.is_open()) {
        cout << "Error: Drift Velocity file not found!" << endl;
        cout << "Aborting" << endl;
	    gApplication->Terminate();
    }

    int runN;
    double DV;
    int counts;

    bool dvGood = false;

    while(dvFile >> runN >> DV >> counts) {
        if(runN == fRunNo) {
            fDriftVelocity = DV;
            dvGood = true;
            continue;
        }
    }

    if(!dvGood) {
        cout << "Error: Run " << fRunNo << " is not in the drift velocity file!" << endl;
        cout << "Aborting" << endl;
	    gApplication->Terminate();
    }

    ifstream ggTBFile;
    if(fRunNo < 1600)
        ggTBFile.open("/data/RB230064/teajun/ggPos/GGD_tb_124Xe.txt");
    else
        ggTBFile.open("/data/RB230064/teajun/ggPos/GGD_tb_136Xe.txt");

    if(!ggTBFile.is_open()) {
        cout << "Error: GG TB file not found!" << endl;
        cout << "Aborting" << endl;
	    gApplication->Terminate();
    }

    double TB;

    bool tbGood = false;

    while(ggTBFile >> runN >> TB) {
        if(runN == fRunNo) {
            fGGtb = TB;
            tbGood = true;
            continue;
        }
    }

    if(!tbGood) {
        cout << "Error: Run " << fRunNo << " is not in the GG TB file!" << endl;
        cout << "Aborting" << endl;
	    gApplication->Terminate();
    }

    double fTBTime = 40;
    double fTbToYConv = -fTBTime * fDriftVelocity / 100;

    double fGGpos = fGGtb * fTbToYConv;
    double fYPedestalOffset = fGGpos - 16;
    cout << "Y Pedestal Offset: " << fYPedestalOffset << endl;



	auto fParameterFile = "ST.parameters.2024Spring.par";





	//auto fBDCOffsetY = -181.8; // Same for Spring and Fall
	//auto fBDCOffsetX = -2.70; // Fall Value




    
	auto fGGRunID = 1177;

	//TString ggDataPathWithFormat = "/data/RB230064/common_2024/ggNoise/ggNoise%d.root";

    TString fBeamData = Form("/data/RB230064/common_2024/bdc_files_2025/bdc_%04d.root", fRunNo);
    TString fGGData = Form("/data/RB230064/common_2024/ggNoise/ggNoise%04d.root", fGGRunID);

	cout << "done with pars" << endl;

	Int_t start = fSplitNo * fNumEventsInSplit;
	TString sRunNo = TString::Itoa(fRunNo, 10);
	TString sSplitNo = TString::Itoa(fSplitNo, 10);

	fPathToData = TString::Format("%s", fPathToData.Data()); // Data to be stored
	if (gSystem->AccessPathName(fPathToData.Data(), kFileExists)) {
		gSystem->mkdir(fPathToData.Data());
	}

	TString version = getVersion((fSpiRITROOTPath + "VERSION.compiled").Data());

	TString par = fSpiRITROOTPath + "parameters/" + fParameterFile;
	TString geo = fSpiRITROOTPath + "geometry/geomSpiRIT.man.root";
	TString fRawDataList = "";
	cout<<"IN the middle"<<endl;
	fRawDataList = TString::Format("./list_run%04d.txt", fRunNo);
	TString out = TString::Format("%srun%04d_s%02d_sc_par.%s.root", fPathToData.Data(), fRunNo, fSplitNo, fOutForm.Data());
	TString log = TString::Format("%srun%04d_s%02d.%s.log", fPathToData.Data(), fRunNo, fSplitNo, fOutForm.Data());

	if (TString(gSystem->Which(".", fRawDataList)).IsNull() && !fUseMeta) {
		if (fIsFRIBDAQ) {
			gSystem->Exec("./createList_FRIBDAQ.sh " + sRunNo);
		} else {
			gSystem->Exec("./createList.sh " + sRunNo);
		}
	}
	TString fMetaDataList;
	if (fUseMeta) {
		cout<<"USING META DATA"<<endl;
		fRawDataList = Form("%s/run_%04d/dataList.txt", fSupplePath.Data(), fRunNo);
		fMetaDataList = Form("%s/run_%04d/metadataList.txt", fSupplePath.Data(), fRunNo);
	}

	FairLogger *logger = FairLogger::GetLogger();

	auto parReader = new FairParAsciiFileIo();
	parReader->open(par);

	auto run = new FairRunAna();
	run->SetGeomFile(geo);
	run->SetOutputFile(out);
	run->GetRuntimeDb()->setSecondInput(parReader);

	auto decoder = new STDecoderTask();
	if (fIsFRIBDAQ) {
		decoder->SetUseFRIBDAQData();
	}
	decoder->SetUseSeparatedData(true);
	decoder->SetPersistence(false); //Taras mod
	decoder -> SetUseGainCalibration(true);

	decoder -> SetGainCalibrationData(gainCalibFile.Data());
	decoder -> SetGainReference(constant, linear, quadratic);
	decoder->SetGGNoiseData(fGGData);
	cout << fRawDataList << endl;
	cout << "setting data list" << endl;
	decoder->SetDataList(fRawDataList);
	cout << "data list set" << endl;
	decoder->SetTbRange(30, 257);
	// Low gain calibration. Don't forget you need to uncomment PSA part, too.
	decoder->SetEventID(start);

	auto preview = new STEventPreviewTask();
	//preview->SetPersistence(true);

	if (fUseMeta) {
		std::ifstream metalistFile(fMetaDataList.Data());
		TString dataFileWithPath;
		for (Int_t iCobo = 0; iCobo < 12; iCobo++) {
			dataFileWithPath.ReadLine(metalistFile);
			dataFileWithPath = Form("%s/run_%04d/%s", fSupplePath.Data(), fRunNo, dataFileWithPath.Data());
			decoder->SetMetaData(dataFileWithPath, iCobo);
		}
	}

    STLinkDAQTask *linker = new STLinkDAQTask();
    linker -> SetInputTree(fBeamData, "TBDC");
    linker -> SetPersistence(true);
    linker -> SetSearchRadius(1e-4);
    linker->SetFirstEvent(start);

	auto psa = new STPSAETask();

	psa->SetPersistence(false); //This parameter is set to "False" for 2016 data
    psa->SetDriftVelocity(fDriftVelocity);
	
	// Pulse having long tail
	psa->SetPulserData("pulser_117ns_50tb.dat");
	// Statistically matching time jitter in different CoBos
  	psa -> SetYOffsets(fSpiRITROOTPath + "parameters/yOffsetCalibration.dat");
    // Used to set 0 in Y properly (tb 0 is not the pad plane)
    psa -> SetYPedestalOffset(fYPedestalOffset); // unit: mm
	
	auto helix = new STHelixTrackingTask();
	helix->SetPersistence(false);
	helix->SetClusterPersistence(false); //And this parameter is set to "False"
	//Left, right, top and bottom sides cut
	helix->SetClusterCutLRTB(420, -420, -64, -522);
	
	auto correct = new STCorrectionTask(); //Correct for saturation

	auto gfBField = STGFBField::GetInstance("samurai_field_map", "A", fFieldOffsetX, fFieldOffsetY, fFieldOffsetZ);   
	double fSheetChargeDensity = p0; //initial value
	//double fSheetChargeDensity = densNum * 2 * 1e-11;

    auto spaceCharge = new STSpaceChargeCorrectionTask();
  	spaceCharge -> SetBField(gfBField -> GetFieldMap());
    //spaceCharge -> SetDriftParameters(-4.355e4, -2.18); // omega tau and mu of the Langevin equation //obsolete 07/29/26
	spaceCharge -> SetDriftVelocity(fDriftVelocity); // use run dependent drift velocity
	spaceCharge -> SetUseComputedYDisplacement(fUseComputedYDisplacementInMap);
  	spaceCharge -> SetApplyYCorrection(fApplyYCorrectionToClusters);
	//const double fLeakageChargeFactor = 10.2;
    const double fLeakageChargeFactor = 0;
   	spaceCharge -> SetSheetChargeDensity(fSheetChargeDensity, fLeakageChargeFactor*fSheetChargeDensity);
   	spaceCharge -> SetProjectile(STSpaceCharge::Projectile::Xe136);  //Test for cocktail beam 
	if (fRunNo < 1600) {
		spaceCharge -> SetProjectile(STSpaceCharge::Projectile::Xe124);  //Test for cocktail beam 
	}
	spaceCharge -> SetElectronDrift(true); 
    //spaceCharge -> SetLocalRate(fSheetChargeDensity, TString::Format("RatesSup/run%04d.root", fRunNo));

    //spaceCharge -> SetLocalRate(0, fSheetChargeDensity, TString::Format("RatesSup/run%04d.root", fRunNo));
	int sbatch_size=100;
	//spaceCharge->SetLocalRate(0,fSheetChargeDensity,Form("RatesSup_sbatch/run%04d.root",fRunNo),Form("timeHist%d",sbatch_size),sbatch_size);
	spaceCharge->SetLocalRate(p1,p0,Form("/data/RB230064/teajun/RatesSup_sbatch/run%04d.root",fRunNo),Form("timeHist%d",sbatch_size),sbatch_size);
	
	
	auto genfitPID = new STGenfitPIDTask();
	// In the TPC frame. Here the z position is used when Genfit do the extrapolation.
	genfitPID->SetTargetPlane(0, 0, fTargetZ); // unit: mm
	genfitPID->SetPersistence(true);
	genfitPID->SetBDCFile("");
	genfitPID->SetListPersistence(true);


	auto fVATargetZ = fTargetZ;
	auto fVATargetZSigma = 1.56; //change as needed 
	auto fVATargetZSigmaMultiple = 3.0;


	auto genfitVA = new STGenfitVATask();
	genfitVA->SetIs2024Data();
	genfitVA->SetPersistence(true);
	genfitVA->SetListPersistence(false);

	///////////////
  	genfitVA->SetTargetPlane(0, -205.5, fTargetZ); // unit: mm - match STGenfitPIDTask default Y position. Updated on 06/18/2026 ; before it was -213.3
  	genfitVA->SetZtoProject(fVATargetZ, fVATargetZSigma, fVATargetZSigmaMultiple); // Added 06/17/2026 Previous implementation was just SetTargetPlane(0, 0, fVATargetZ); This should now be setting the cluster at z vertex position as a hard constraint

	genfitVA->SetBeamFile(fBeamData);
    genfitVA->SetInformationForBDC(fRunNo, fBDCOffsetX, fBDCOffsetY, 0);
	// Uncomment if you want to recalculate the vertex using refit tracks.
	genfitVA->SetUseRave(true);
	// genfitVA -> ShiftBDCAfterSC("../parameters/vertex_shift_Sn" + std::to_string(int(fSystemID/1000)) + ".root", 80);
	//genfitVA->SetZtoProject(
	//	-21.3, 2.24, 3
	//); //(Double_t peakZ, Double_t sigma, Double_t sigmaMultiple), this function will project the BDC on the Target.

	run->AddTask(decoder);
    run->AddTask(linker);
	run->AddTask(preview);
	run->AddTask(psa);
	run->AddTask(helix);
	run->AddTask(correct);
	run->AddTask(spaceCharge);
	run->AddTask(genfitPID);
	run->AddTask(genfitVA);

	run->Init();
	cout << "processing run " << fRunNo << " with " << fNumEventsInSplit << " events." << endl;
	run->Run(0, fNumEventsInSplit);

	cout << "Log    : " << log << endl;
	cout << "Input  : " << fRawDataList << endl;
	cout << "Output : " << out << endl;

	gApplication->Terminate();
}

TString getVersion(const std::string &pth) {
	std::string version;
	std::ifstream vfile(pth.c_str());
	vfile >> version;
	vfile.close();
	return TString(version.c_str());
}
