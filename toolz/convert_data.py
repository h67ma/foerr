from consts import *

# more meaningful location names translations
loc_names_map = {
	# "rooms2": "", # test loc
	"rooms_art": "fort_rocks",
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

# wake me up insideeeee
mat_translations_map = {
	"\u041b\u0435\u0441\u0442\u043d\u0438\u0446\u0430 \u0441\u043f\u0440\u0430\u0432\u0430": "ladder_right",
	"\u041b\u0435\u0441\u0442\u043d\u0438\u0446\u0430 \u0441\u043b\u0435\u0432\u0430": "ladder_left",
	"\u0421\u0442\u0443\u043f\u0435\u043d\u044c\u043a\u0438 /": "stairs_up",
	"\u0421\u0442\u0443\u043f\u0435\u043d\u044c\u043a\u0438 \\\\": "stairs_down",
	"\u0414\u0435\u0440\u0435\u0432\u044f\u043d\u043d\u044b\u0435 \u0441\u0442. /": "wooden_stairs_up",
	"\u0414\u0435\u0440\u0435\u0432\u044f\u043d\u043d\u044b\u0435 \u0441\u0442. \\\\": "wooden_stairs_down",
	"\u0421\u0442\u0430\u043b\u044c\u043d\u044b\u0435 \u0441\u0442. /": "steel_stairs_up",
	"\u0421\u0442\u0430\u043b\u044c\u043d\u044b\u0435 \u0441\u0442. \\\\": "steel_stairs_down",
	"\u0411\u0435\u0442\u043e\u043d\u043d\u044b\u0435 \u0441\u0442. /": "concrete_stairs_up",
	"\u0411\u0435\u0442\u043e\u043d\u043d\u044b\u0435 \u0441\u0442. \\\\": "concrete_stairs_down",
	"\u041a\u0430\u043d\u0442 \u0441\u0442. /": "canterlot_stairs_up",
	"\u041a\u0430\u043d\u0442 \u0441\u0442. \\\\": "canterlot_stairs_down",
	"\u041e\u0431\u043b\u0430\u0447\u043d\u044b\u0435 \u0441\u0442. /": "cloud_stairs_up",
	"\u041e\u0431\u043b\u0430\u0447\u043d\u044b\u0435 \u0441\u0442. \\\\": "cloud_stairs_down",
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
}

flag_part_height = [
	',', # 3/4 cell height
	';', # 1/2 cell height
	':', # 1/4 cell height
]

missing_mats = {
	'*': {
		FOERR_JSON_KEY_TYPE: 6,
		FOERR_JSON_KEY_COLOR: "#0064C9",	# blue (default)
		FOERR_JSON_KEY_TEXTURE_DELIM: "liquid_water_delim",
	},
	'&': {
		FOERR_JSON_KEY_TYPE: 6,
		FOERR_JSON_KEY_COLOR: "#76982E",	# green
		FOERR_JSON_KEY_TEXTURE_DELIM: "liquid_goo_delim",
	},
	'^': {
		FOERR_JSON_KEY_TYPE: 6,
		FOERR_JSON_KEY_COLOR: "#383E41",	# black (??)
		FOERR_JSON_KEY_TEXTURE_DELIM: "liquid_tar_delim",
	},
	'%': {
		FOERR_JSON_KEY_TYPE: 6,
		FOERR_JSON_KEY_COLOR: "#FF66AA",	# pink
		FOERR_JSON_KEY_TEXTURE_DELIM: "liquid_pink_delim",
	},
}

# maps "wtip" to liquid material symbol
liquid_type_map = {
	0: '*',	# blue (default)
	1: '&',	# green
	2: '^',	# black (??)
	3: '%',	# pink
}

# materials not actually used in any room
mat_blacklist = [
	"100",	# border solid
]

LADDER_LEFT_OFFSET_LEFT = -2
LADDER_RIGHT_OFFSET_LEFT = 21
LADDER_LEFT_DELIM_OFFSET = [-2, -30]
LADDER_RIGHT_DELIM_OFFSET = [2, -30]

STAIRS_OFFSETS_LEFT = {
	"canterlot_stairs_up": -3,
	"cloud_stairs_up": -2,
	"steel_stairs_up": -4,
	"wooden_stairs_up": -4,
}
