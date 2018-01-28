//    CSA_005.dcl 
//    Sample dialogue code
dcl_settings : default_dcl_settings { audit_level = 0; }

csa_005 : dialog {
  label = "Levels    (005)";
  : column {
    : row {
      : list_box {
        label = "No.  Level   Text for Elevation ";
        tabs =  "2 6 13";
        key =   "sizebox";
        list =  " 2   3250    Top of Footing RL 100.950";
        multiple_select = false;
        width = 50;
      }
    }
  }
  : row {
    ok_cancel;
  }
}
