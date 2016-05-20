INSERT OR IGNORE INTO `usage_and_error_count` (
  `count_of_iap_buffer_full`, `count_sync_out_of_memory`,
  `count_of_sync_reboots`) VALUES (0, 0, 0);
INSERT OR IGNORE INTO `module_meta` (`pt_exchanged_at_odometer_x`,
  `pt_exchanged_x_days_after_epoch`, `ignition_cycles_since_last_exchange`,
  `flag_update_required`)
  VALUES (0, 0, 0, 0);
INSERT OR IGNORE INTO `module_config` (`preloaded_pt`, `is_first_run`,
  `exchange_after_x_ignition_cycles`, `exchange_after_x_kilometers`,
  `exchange_after_x_days`, `timeout_after_x_seconds`)
  VALUES(1, 1, 0, 0, 0, 0);
INSERT OR IGNORE INTO `priority`(`value`) VALUES ('EMERGENCY');
INSERT OR IGNORE INTO `priority`(`value`) VALUES ('NAVIGATION');
INSERT OR IGNORE INTO `priority`(`value`) VALUES ('VOICECOMMUNICATION');
INSERT OR IGNORE INTO `priority`(`value`) VALUES ('COMMUNICATION');
INSERT OR IGNORE INTO `priority`(`value`) VALUES ('NORMAL');
INSERT OR IGNORE INTO `priority`(`value`) VALUES ('NONE');
INSERT OR IGNORE INTO `hmi_level`(`value`) VALUES ('FULL');
INSERT OR IGNORE INTO `hmi_level`(`value`) VALUES ('LIMITED');
INSERT OR IGNORE INTO `hmi_level`(`value`) VALUES ('BACKGROUND');
INSERT OR IGNORE INTO `hmi_level`(`value`) VALUES ('NONE');
INSERT OR IGNORE INTO `version` (`number`) VALUES('0');