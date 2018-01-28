//    CSA_010.dcl 
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
csa_010 : dialog {
  label = "Manual Operation    (010)";
  : column {
    : boxed_column {
      label = "Current Status";
      : row {
        : button {
          label = "    Job  ...";
          mnemonic = "J";
          key = "moj";
        }
        : edit_box {
          label = "";
          value = "\\SD3\\92001";
          edit_width = 20;
        }
        spacer_0;
      }
      : row {
        : button {
          label = "Wireframe...";
          mnemonic = "W";
          key = "mow";
        }
        : edit_box {
          label = "";
          value = "\\SD3\\WF1";
          edit_width = 20;
        }
        spacer_0;
      }
      : row {
        : button {
          label = "   Area  ...";
          mnemonic = "A";
          key = "moa";
        }
        : edit_box {
          label = "";
          value = "A ";
          edit_width = 20;
        }
        spacer_0;
      }
    }
    : boxed_column {
      label = "Functions";
      : button {
        label = "1  Configuration ...    ";
        mnemonic = "C";
        key = "mo_c";
      }
      : button {
        label = "2  Wireframe ...        ";
        mnemonic = "i";
        key = "mo_i";
      }
      : button {
        label = "3  Members ...          ";
        mnemonic = "M";
        key = "mo_m";
      }
      : button {
        label = "4  Single Connection ...";
        mnemonic = "S";
        key = "mo_s";
      }
      : button {
        label = "5  Connection Group ... ";
        mnemonic = "G";
        key = "mo_g";
      }
      : button {
        label = "6  Drawings ...         ";
        mnemonic = "D";
        key = "mo_d";
      }
      : button {
        label = "7  Extra Functions ...  ";
        mnemonic = "x";
        key = "mo_x";
      }
      : button {
        label = "8  Plot Manager ...     ";
        mnemonic = "L";
        key = "mo_l";
      }
      : button {
        label = "9  Material Takeoff ... ";
        mnemonic = "T";
        key = "mo_t";
      }
    }
  }
  cancel_help;
}
