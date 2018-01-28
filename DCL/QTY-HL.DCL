//----------------------------------------------------------------------------
//
//  qty-hl.dcl - quantity of holes insertion
//
//----------------------------------------------------------------------------

dcl_settings : default_dcl_settings { audit_level = 0; }

ddqty_hl : dialog {
  aspect_ratio = 0;
  label = "Hole Mark";
  initial_focus = "qty_hl_qty";
  horizontal_alignment = left;
  : row {
    spacer_1;
    : edit_box {
      mnemonic             = "Q";
      label                = "Quantity: ";
      key                  = "qty_hl_qty";
      edit_width           = 6;
      allow_accept         = true;
    }
    spacer_2;
  }
  : row {
    spacer_1;
    : edit_box {
      mnemonic             = "H";
      label                = "Hole size:";
      key                  = "qty_hl_siz";
      edit_width           = 6;
      allow_accept         = true;
    }
    spacer_2;
  }
  spacer_1;
  : row {
    : radio_column {
      label = "Places to";
      : radio_button {
        label = "Far-Side";
        mnemonic = "F";
        key = "qty_hl_frs";
      }
      : radio_button {
        label = "Near-side";
        mnemonic = "N";
        key = "qty_hl_nrs";
      }
      : radio_button {
        label = "Cross-centres";
        mnemonic = "C";
        key = "qty_hl_crs";
      }
      : radio_button {
        label = "Centre-Line";
        mnemonic = "L";
        key = "qty_hl_ctl";
      }
      : radio_button {
        label = "None";
        mnemonic = "o";
        key = "qty_hl_non";
      }
    }
  }
  : boxed_column {
    : row {
      : edit_box {
        label                = " Dim f/s: ";
        key                  = "qty_hl_str_frs";
        edit_width           = 6;
        allow_accept         = true;
      }
    }
    : row {
      : edit_box {
        label                = " Dim n/s: ";
        key                  = "qty_hl_str_nrs";
        edit_width           = 6;
        allow_accept         = true;
      }
    }
    : row {
      : edit_box {
        label                = " Dim c/crs:";
        key                  = "qty_hl_str_crs";
        edit_width           = 6;
        allow_accept         = true;
      }
    }
  }
  spacer_1;
  ok_cancel;
}
