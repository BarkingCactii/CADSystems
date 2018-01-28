//    CSA_006.dcl 
//    Sample dialogue code
dcl_settings : default_dcl_settings { audit_level = 0; }

csa_006 : dialog {
  label = "Draw Single Workline   (006)";
  : column {
    : boxed_column {
      label = "Set UCS";
      : radio_button {
        label = "WCS";
        value = "1";
        mnemonic = "W";
        key = "ucs_w";
      }
      : radio_button {
        label = "Front";
        mnemonic = "F";
        key = "ucs_f";
      }
      : radio_button {
        label = "Back";
        mnemonic = "B";
        key = "ucs_b";
      }
      : radio_button {
        label = "Left";
        mnemonic = "L";
        key = "ucs_l";
      }
      : radio_button {
        label = "Right";
        mnemonic = "R";
        key = "ucs_r";
      }
      : radio_button {
        label = "Other";
        mnemonic = "O";
        key = "ucs_o";
      }
      : boxed_column {
        : button {
          label = "Other...";
          key = "mnu_o";
        }
        : edit_box {
          label = "";
          value = "User1";
          key = "ucs_h";
          edit_width = 8;
        }
      }
    }
    : button {
      label = "Pick 2 points...";
      key = "ds_2p";
    }
  }
  ok_cancel;
}
