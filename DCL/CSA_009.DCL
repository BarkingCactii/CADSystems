//    CSA_009.dcl 
//    Sample dialogue code
dcl_settings : default_dcl_settings { audit_level = 0; }

cancel_help : column {
  : row {
    fixed_width = true;
    alignment = centered;
    cancel_button;
    : spacer { width = 2; }
    help_button;
  }
}
csa_009 : dialog {
  label = "Wireframe Operation    (009)";
  : column {
    : boxed_column {
      label = "Current Status";
      : row {
        : button {
          label = "      Job...";
          mnemonic = "J";
          key = "wo_j";
        }
        : edit_box {
          label = "";
          value = "\\SD3\\92001";
          edit_width = 22;
        }
        spacer_0;
      }
      : row {
        : button {
          label = "Wireframe...";
          mnemonic = "W";
          key = "wo_w";
        }
        : edit_box {
          label = "";
          value = "WF1";
          edit_width = 22;
        }
        spacer_0;
      }
      : row {
        : button {
          label = "     Area...";
          mnemonic = "A";
          key = "wo_a";
        }
        : edit_box {
          label = "";
          value = "A";
          edit_width = 22;
        }
        spacer_0;
      }
    }
    : boxed_column {
      label = "Steps";
      : button {
        label = " 1 Define Wireframe (WF)...        ";
        mnemonic = "D";
        key = "wo_d";
      }
      : button {
        label = " 2 Add Members to Wireframe...     ";
        mnemonic = "M";
        key = "wo_m";
      }
      : button {
        label = " 3 Add Single Conn to Member...    ";
        mnemonic = "S";
        key = "wo_s";
      }
      : button {
        label = " 4 Add Conn Group to Wireframe...  ";
        mnemonic = "G";
        key = "wo_g";
      }
      : button {
        label = " 5 Set Member Presentation...      ";
        mnemonic = "P";
        key = "wo_p";
      }
      : button {
        label = " 6 Detail Members on Drawings...   ";
        mnemonic = "e";
        key = "wo_e";
      }
      : button {
        label = " 7 Detail Fittings on drawings...  ";
        mnemonic = "F";
        key = "wo_f";
      }
      : button {
        label = " 8 Plot Drawings...                ";
        mnemonic = "L";
        key = "wo_l";
      }
      : button {
        label = " 9 Issue Drawings...               ";
        mnemonic = "I";
        key = "wo_i";
      }
      : button {
        label = "10 Quantity/Bolt List Extraction...";
        mnemonic = "Q";
        key = "wo_q";
      }
    }
  }
  cancel_help;
}
