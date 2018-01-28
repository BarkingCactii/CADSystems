//    CSA_003.dcl 
//    Sample dialogue code
dcl_settings : default_dcl_settings { audit_level = 0; }

csa_003 : dialog {
  label = "X-Bay Spacing    (003)";
  : column {
    : row {
      : list_box {
        label = "No.  X-Spacing   Symbol ";
        tabs = "2 6 13";
        key = "sizebox";
        list = " 2    3750       B";
        multiple_select = false;
        width = 16;
      }
    }
  }
  : row {
    ok_cancel;
  }
}
