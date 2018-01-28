//----------------------------------------------------------------------------
//
//  qty-mk.dcl - quantity marks
//
//----------------------------------------------------------------------------

dcl_settings : default_dcl_settings { audit_level = 0; }

ddqty_mk : dialog {
  aspect_ratio = 0;
  label = "Quantity Marks";
  initial_focus = "qty_mk_qty";
  horizontal_alignment = left;
  spacer_1;
  : row {
    : edit_box {
      mnemonic             = "Q";
      label                = "Quantity:";
      key                  = "qty_mk_qty";
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
        label = "Drawn";
        mnemonic = "D";
        key = "qty_mk_drw";
      }
      : radio_button {
        label = "Noted";
        mnemonic = "N";
        key = "qty_mk_not";
      }
      : radio_button {
        label = "Opposite";
        mnemonic = "O";
        key = "qty_mk_opp";
      }
      : radio_button {
        label = "Drawn & Noted";
        mnemonic = "r";
        key = "qty_mk_dan";
      }
      : radio_button {
        label = "Opposite & Noted";
        mnemonic = "r";
        key = "qty_mk_oan";
      }
      : radio_button {
        label = "None";
        mnemonic = "o";
        key = "qty_mk_non";
      }
    }
  }
  spacer_1;
  : row {
    horizontal_alignment = left;
    : edit_box {
      mnemonic             = "M";
      label                = "Mark:";
      key                  = "qty_mk_mrk";
      edit_width           = 10;
      allow_accept         = true;
    }
    spacer_1;
  }
  spacer_1;
  ok_cancel;
}


