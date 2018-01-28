//----------------------------------------------------------------------------
//
//  cope.dcl - cope
//
//----------------------------------------------------------------------------

dcl_settings : default_dcl_settings { audit_level = 0; }

ddcope : dialog {
  aspect_ratio = 0;
  label = "Cope";
  initial_focus = "cope_width";
  horizontal_alignment = left;
  : row {
    : radio_column {
      label = "Corner to cope";
      : radio_button {
        label = "Top Left";
        key = "cope_tl";
      }
      : radio_button {
        label = "Top right";
        key = "cope_tr";
      }
      : radio_button {
        label = "Bottom left";
        key = "cope_bl";
      }
      : radio_button {
        label = "Bottom right";
        key = "cope_br";
      }
    }
  }
  : boxed_column {
    horizontal_alignment = center;
    : edit_box {
      label                = "Cope Width: ";
      key                  = "cope_width";
      edit_width           = 6;
      allow_accept         = true;
    }
    : edit_box {
      label                = "Cope Depth: ";
      key                  = "cope_depth";
      edit_width           = 6;
      allow_accept         = true;
    }
  }
  spacer_1;
  : row {
    horizontal_alignment = center;
    spacer_1;
    : toggle {
      label                = "Add Dimensions";
      key                  = "cope_dim";
    }
  }
  spacer_1;
  ok_cancel;
}


