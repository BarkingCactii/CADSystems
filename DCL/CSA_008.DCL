//    CSA_008.dcl 
//    Sample dialogue code
dcl_settings : default_dcl_settings { audit_level = 0; }

csa_008 : dialog {
  label = "Start Options            (008)";
  : boxed_column {
    label = "Operation";
    : radio_button {
      label = "Wireframe";
      value = "1";
      key = "so_wf";
      mnemonic = "W";
    }
    : radio_button {
      label = "Manual";
      mnemonic = "M";
      key = "so_man";
    }
    : radio_button {
      label = "Modify";
      mnemonic = "y";
      key = "so_mod";
    }
  }
  : row {
    ok_cancel_help;
  }
}
