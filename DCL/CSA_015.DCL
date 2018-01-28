//    CSA_015.dcl 
//    Sample dialogue code
dcl_settings : default_dcl_settings { audit_level = 0; }

csa_015 : dialog {
  label = "Current Wireframe   (015)";
  : column {
    label = "Wireframe Directory";
    : edit_box {
      label = "";
      width = "30";
      value = "\\SD3\\92001";
    }
    : list_box {
      label = "Wireframe Files";
      width = "20";
      list = "WF1\nWF2\nWF4\nWF6\nWF9\nWF10";
      value = "2";
    }
    : edit_box {
      label = "Wireframe";
      key = "cw_w";
      value = "WF4";
      edit_width = "8";
    }
  }
  ok_cancel_help;
}
