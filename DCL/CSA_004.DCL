//    CSA_004.dcl 
//    Sample dialogue code
dcl_settings : default_dcl_settings { audit_level = 0; }

csa_004 : dialog {
  label = "Y-Bay Spacing    (004)";
  : column {
    : row {
      : list_box {
        label = "No.  Y-Spacing   Symbol ";
        tabs = "2 6 13";
        key = "sizebox";
        list = " 3   4250        5";
        multiple_select = false;
        width = 16;
      }
    }
  }
  : row {
    ok_cancel;
  }
}
