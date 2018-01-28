//    CSA_007.dcl 
//    Sample dialogue code
dcl_settings : default_dcl_settings { audit_level = 0; }

csa_007 : dialog {
  label = "Insert External WF File     (007)";
  : column {
    : column {
      : edit_box {
        label = "Name of File";
        value = "\\SD3\\92002\\WF2";
        key = "ix_wf";
        edit_width = 40;
      }
      : row {
        : list_box {
          label = "Directories";
          multiple_select = false;
          list = "\\SD3\\92001 \n\\SD3\\92002 \n\\SD3\\92003";
          value = "1";
          key = "ix_d";
        }
        : list_box {
          label = "Files";
          multiple_select = false;
          list = "WF1 \nWF2 \nWF3 \nWF5 \nWF11";
          value = "2";
          key = "ix_f";
        }
      }
    }
  }
  ok_cancel;
}
