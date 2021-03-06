#include "QBBCCMS.hh"
#include "SimG4Core/PhysicsLists/interface/CMSMonopolePhysics.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "G4EmStandardPhysics_option1.hh"
#include "G4DecayPhysics.hh"
#include "G4EmExtraPhysics.hh"
#include "G4StoppingPhysics.hh"
#include "G4HadronicProcessStore.hh"

#include "G4DataQuestionaire.hh"
#include "G4HadronInelasticQBBC.hh"
#include "G4HadronElasticPhysicsXS.hh"
#include "G4IonPhysics.hh"
#include "G4NeutronTrackingCut.hh"

QBBCCMS::QBBCCMS(G4LogicalVolumeToDDLogicalPartMap& map, 
		 const HepPDT::ParticleDataTable * table_, 
		 sim::FieldBuilder *fieldBuilder_, 
		 const edm::ParameterSet & p) : PhysicsList(map, table_, fieldBuilder_, p) {

  G4DataQuestionaire it(photon);
  
  int  ver     = p.getUntrackedParameter<int>("Verbosity",0);
  bool emPhys  = p.getUntrackedParameter<bool>("EMPhysics",true);
  bool hadPhys = p.getUntrackedParameter<bool>("HadPhysics",true);
  bool tracking= p.getParameter<bool>("TrackingCut");
  bool munucl  = p.getParameter<bool>("FlagMuNucl");
  edm::LogInfo("PhysicsList") << "You are using the simulation engine: "
			      << "QBBC with Flags for EM Physics "
			      << emPhys << " and for Hadronic Physics "
			      << hadPhys 
			      << " and tracking cut " << tracking;

  if (emPhys) {
    // EM Physics
    RegisterPhysics( new G4EmStandardPhysics_option1(ver));

    // Synchroton Radiation & GN Physics
    G4EmExtraPhysics* gn = new G4EmExtraPhysics(ver);
    if(munucl) { G4String yes = "on"; gn->MuonNuclear(yes); }
    RegisterPhysics(gn);
  }

  // Decays
  RegisterPhysics(new G4DecayPhysics(ver));

  if (hadPhys) {
    G4HadronicProcessStore::Instance()->SetVerbose(ver);

    // Hadron Elastic scattering
    RegisterPhysics(new G4HadronElasticPhysicsXS(ver));

    // Hadron Physics
    RegisterPhysics( new G4HadronInelasticQBBC(ver));

    // Stopping Physics
    RegisterPhysics(new G4StoppingPhysics(ver));

    // Ion Physics
    RegisterPhysics(new G4IonPhysics(ver));

    // Neutron tracking cut
    if (tracking) {
      RegisterPhysics( new G4NeutronTrackingCut(ver));
    }
  }

  // Monopoles
  RegisterPhysics( new CMSMonopolePhysics(table_,fieldBuilder_,p));
}

