# more meaningful location names translations
loc_names_map = {
	# "rooms2": "", # test loc
	"rooms_art": "fort_focks",
	"rooms_begin": "technical_tunnels",
	"rooms_canter": "canterlot_ruins",
	"rooms_comm": "command_post",
	"rooms_core": "stable_88_core",
	"rooms_covert": "covertsign",
	"rooms_encl": "enclave_base",
	"rooms_garages": "hankey_garages",
	"rooms_grave": "funeral",
	"rooms_hql": "legion_hq",
	"rooms_mane": "manehattan_ruins",
	"rooms_mbase": "bunker", # or military_base
	"rooms_minst": "moi",
	"rooms_mtn": "mas",
	"rooms_nio": "dsr",
	"rooms_pis": "stable_pi_entrance",
	"rooms_pi": "stable_pi", # or stable_pi_atk
	"rooms_plant": "factory",
	"rooms_post": "enclave_outpost",
	"rooms_prob": "trials_bosses",
	"rooms_raiders": "falls_springs", # aka Raider's Den
	"rooms_rbl": "prancing_pony",
	"rooms_sewer": "sewers",
	"rooms_src": "water_treatment_plant",
	"rooms_stable": "abandoned_stable",
	"rooms_surf": "surface",
	"rooms_thunder": "thunderhead",
	"rooms_way": "way_to_canterlot", # Askari meet
	"rooms_workshop": "gun_workshop",
	# "z_shablon": "", # room template
}

# more meaningful background full names
# GameData -> <land> -> <options fon="...">
bg_full_name_map = {
	"fonCanter": "canterlot",
	"fonClear": "desert_day",
	"fonDarkClouds": "surface",
	"fonEnclave": "clouds",
	"fonFinalb": "its_a_fine_night",
	"fonFire": "hell_clouds",
	"fonRuins": "manehattan",
	"fonWay": "crystal_mountains",
}

# mapping for replacing non-ASCII characters in room data
symbol_trans_map = {
	# platforms
	'-': 'a',
	'\u0414': 'b',
	'\u0415': 'c',
	'\u041a': 'd',
	'\u041d': 'e',
	'\u0420': 'f', # blaze it

	# ladders
	'\u0410': 'g',
	'\u0411': 'h',

	# stairs
	'\u0412': 'i',
	'\u0413': 'j',
	'\u0416': 'k',
	'\u0417': 'l',
	'\u0418': 'm',
	'\u0419': 'n',
	'\u041b': 'o',
	'\u041c': 'p',
	'\u041e': 'q',
	'\u041f': 'r',
	'\u0421': 's',
	'\u0422': 't',

	# border solid
	"100": '~',
}

extra_translations = {
	"Граница": "Border",
}

flag_part_height = [
	',', # 3/4 cell height
	';', # 1/2 cell height
	':', # 1/4 cell height
]

FLAG_WATER = '*'

missing_mats = {
	FLAG_WATER: {
		"type": 6
	}
}
