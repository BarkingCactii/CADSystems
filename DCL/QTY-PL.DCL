//----------------------------------------------------------------------------
//
//  qty-pl.dcl - quantity of places mark
//
//----------------------------------------------------------------------------

dcl_settings : default_dcl_settings { audit_level = 0; }

ddqty_pl : dialog {
  aspect_ratio = 0;
  label = "Place Marks";
  initial_focus = "qty_pl_qty";
  horizontal_alignment = left;
  spacer_1;
  : row {
    : edit_box {
      mnemonic             = "Q";
      label                = "Quantity:";
      key                  = "qty_pl_qty";
      edit_width           = 6;
      allow_accept         = true;
    }
    spacer_1;
  }
  spacer_1;
  : row {
    : radio_column {
      label = "as";
      : radio_button {
        label = "Far-side";
        mnemonic = "F";
        key = "qty_pl_frs";
      }
      : radio_button {
        label = "Near-side";
        mnemonic = "N";
        key = "qty_pl_nrs";
      }
      : radio_button {
        label = "Top";
        mnemonic = "T";
        key = "qty_pl_top";
      }
      : radio_button {
        label = "Bottom";
        mnemonic = "B";
        key = "qty_pl_bot";
      }
      : radio_button {
        label = "Centre-line";
        mnemonic = "C";
        key = "qty_pl_ctl";
      }
      : radio_button {
        label = "None";
        mnemonic = "o";
        key = "qty_pl_non";
      }
    }
  }
  spacer_1;
  ok_cancel;
}


