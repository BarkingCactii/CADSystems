//    CSA_016.dcl 
//    Sample dialogue code
dcl_settings : default_dcl_settings { audit_level = 0; }

yes_button : retirement_button {
  label = "Yes";
  key = "yes";
  mnemonic = "Y";
}
yes_cancel : column {
  : row {
    fixed_width = true;
    alignment = centered;
    yes_button;
    : spacer { width = 2; }
    cancel_button;
  }
}
csa_016 : dialog {
  label = "Wireframe Alert   (016)";
  : paragraph {
    : text_part {
     label = "This WIREFRAME does not exist.";
    }
    : text_part {
      label = "Create NEW wireframe ??";
    }
  }
  yes_cancel;
}
