//    CSA_002.dcl 
//    Sample dialogue code
dcl_settings : default_dcl_settings { audit_level = 0; }

csa_002 : dialog {
  label = "Draw Single-2D Frame    (002)";
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
      : row {
        label = "Width of frame    Left Ht of frame   Right Ht of frame   No. of Bays";
      }
      : row {
        : edit_box {
          label = "";
          value = "3750";
          key = "w_f";
          edit_width = 8;
        }
        spacer_0;
        : edit_box {
          label = "";
          value = "4150";
          key = "lh_f";
          edit_width = 8;
        }
        spacer_0;
        : edit_box {
          label = "";
          value = "4750";
          key = "rh_f";
          edit_width = 8;
        }
        spacer_0;
        : edit_box {
          label = "";
          value = "7";
          key = "no_f";
          edit_width = 3;
        }
      }
      : row {
        : editbox {
          label = "";
          key = "tw_f";
          edit_width = 8;
        }
        : editbox {
          label = "";
          key = "tl_f";
          edit_width = 8;
        }
        : editbox {
          label = "";
          key = "tr_f";
          edit_width = 8;
        }
        : editbox {
          label = "";
          key = "tn_f";
          edit_width = 3;
        }
      }
      : row {
        : list_box {
          label = "Panel  Distance  Diagonal";
          tabs = "2 8 16";
          key = "sizebox";
          list = " 3       2500       K";
          multiple_select = false;
          width = 20;
        }
        : boxed_column {
          label = "Modify Values";
          : edit_box {
            label = "Bay  :";
            value = "2";
            key = "no_bay";
            edit_width = 3;
          }
          : edit_box {
            label = "Distance";
            value = "3500";
            key = "bay_dist";
            edit_width = 8;
          }
          : edit_box {
            label = "Diagonal ";
            value = "K";
            key = "diag_type";
            edit_width = 3;
          }
        }
        : boxed_column {
          label = "Diagonal Legend";
          : row {
            : text {
              label = "None   : N";
            }
            : image_button {
              key = "n_img";
              color = graphics_background;
              height = 1.5;
              width = 2;
            }
          }
          : row {
            : text {
              label = "TL-BR  : T";
            }
            : image_button {
              key = "t_img";
              color = graphics_background;
              height = 1.5;
              width = 2;
            }
          }
          : row {
            : text {
              label = "BL-TR  : B";
            }
            : image_button {
              key = "b_img";
              color = graphics_background;
              height = 1.5;
              width = 2;
            }
          }
          : row {
            : text {
              label = "Cross  : C";
            }
            : image_button {
              key = "c_img";
              color = graphics_background;
              height = 1.5;
              width = 2;
            }
          }
          : row {
            : text {
              label = "KBrace : K";
            }
            : image_button {
              key = "k_img";
              color = graphics_background;
              height = 1.5;
              width = 2;
            }
          }
        }
      }
    }
  }
  : row {
    ok_cancel;
  }
}
