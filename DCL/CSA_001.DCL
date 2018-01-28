//    CSA_001.dcl 
//    Sample dialogue code
dcl_settings : default_dcl_settings { audit_level = 0; }

csa_001 : dialog {
  label = "Draw Multi-3D Frame       (001)";
  : column {
    : row {
      : boxed_column {
        label = "X Direction";
        : row {
          : button {
            label = "X-Bays...";
            key = "xb_dc";
            mnemonic = "-";
          }
          : edit_box {
            label = "";
            value = "3";
            key = "xb_no";
            edit_width = 3;
          }
        }
        :edit_box {
          label = "Start Symbol";
          value = "A";
          key = "xb_ss";
          edit_width = 3;
        }
        : radio_button {
          label = "Left to Right";
          value = "1";
          mnemonic = "L";
          key = "x_lr";
        }
        : radio_button {
          label = "Right to Left";
          mnemonic = "R";
          key = "x_rl";
        }
      }
      : boxed_column {
        label = "Y Direction";
        : row {
          : button {
            label = "Y Bays...";
            key = "yb_dc";
            mnemonic = "a";
          }
          : edit_box {
            label = "";
            value = "7";
            key = "yb_no";
            edit_width = 3;
          }
        }
        : edit_box {
          label = "Start Symbol";
          value = "1";
          key = "yb_ss";
          edit_width = 3;
        }
        : radio_button {
          label = "Top to Btm";
          value = "1";
          mnemonic = "T";
          key = "y_tb";
        }
        : radio_button {
          label = "Btm to Top";
          mnemonic = "B";
          key = "y_bt";
        }
      }
      : boxed_column {
        label = "Levels";
        : row {
          : button {
            label = "Levels...";
            key = "l_dc";
            mnemonic = "L";
          }
          : edit_box {
            label = "";
            value = "3";
            key = "no_lvls";
            edit_width = 3;
          }
        }
        : edit_box {
          label = "Start No.";
          value = "0";
          key = "st_lvno";
          edit_width = 3;
        }
        : edit_box {
          label = "Start RL";
          value = "6.800";
          key = "lv_st";
          edit_width = 8;
        }
      }
    }
    : row {
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
      : column {
        : boxed_column {
          label = "Enable Settings";
          : toggle {
            label = "Remove 1st Level Worklines";
            value = "1";
            mnemonic = "R";
            key = "remove";
          }
          : row {
            : toggle {
              label = "Show Grid Marks  ";
              value = "1";
              mnemonic = "S";
              key = "marking";
            }
            : text {
              label = " Marking grid level";
            }
            : edit_box {
              label = "";
              value = "3";
              key = e_mg;
              edit_width = 3;
            }
          }
          : toggle {
            label = "Add Dimensions to Grid";
            value = "1";
            mnemonic = "A";
            key = "add";
          }
        }
        : boxed_column {
          label = "Mark North";
          : toggle {
            label = "North-point Mark";
            value = "1";
            mnemonic = "N";
            key = "north";
          }
          : radio_button {
            label = "0";
            mnemonic = "0";
            key = "mnd_0";
          }
          : radio_button {
            label = "90";
            value = "1";
            mnemonic = "9";
            key = "mnd_9";
          }
          : radio_button {
            label = "180";
            mnemonic = "1";
            key = "mnd_1";
	  }
	  : radio_button {
	    label = "270";
	    mnemonic = "2";
	    key = "mnd_2";
          }
        }
      }
    }
    ok_cancel;
  }
}
