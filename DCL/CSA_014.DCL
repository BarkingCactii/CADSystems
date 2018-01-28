//    CSA_014.dcl 
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
csa_014 : dialog {
  label = "Draw Worklines   (014)";
  : column {
    : boxed_column {
      label = "Draw Worklines";
      : button {
        label = "Draw Single Workline...";
        key = "dw_dsw";
      }
      : button {
        label = "Draw Single 2-D Frame...";
        key = "dw_dsf";
      }
      : button {
        label = "Draw Multi 3-D Frame...";
        key = "dw_dmf";
      }
    }
    : boxed_column {
      label = "External WF File";
      : button {
        label = "Insert External WF File...";
	key = "dw_ixw";
      }
    }
  }
  cancel_help;
}
