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
	'u': {
		FOERR_JSON_KEY_TYPE: 2,
		FOERR_JSON_KEY_TEXTURE: "tWindows",
	},
	'v': {
		FOERR_JSON_KEY_TYPE: 2,
		FOERR_JSON_KEY_TEXTURE: "tLeaking",
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

UNDEFINED_LAYER_NUMBER = 0

pad_img_data = {
	"back_hole_0_e.png": (13, 9),
	"back_hole_1_e.png": (18, 12),
	"back_hole_2_e.png": (0, 18),
	"back_hole_3_e.png": (14, 6),
	"back_hole_4_e.png": (8, 13),
}

# set this manually, based on extract_svg_offsets.py
obj_offsets = {
	"back_awindow": (-3, 0),
	"back_barrow": (6, 14),
	"back_battery": (1, 0),
	"back_bio": (0, 6),
	"back_blood1": (-8, 0),
	"back_blood2": (0, 0),
	"back_books": (2, 0),
	"back_bvent": (-5, 0),
	"back_celest": (40, 0),
	"back_chert": (-20, -20),
	"back_clock": (-5, -3),
	"back_door": (12, 20),
	"back_electro": (2, 0),
	"back_enclave": (-10, 0),
	"back_fuse": (-2, 0),
	"back_fwindow": (-5, 0),
	"back_generator": (1, 0),
	"back_h109": (20, 20),
	"back_heap1": (0, -2),
	"back_heap2": (0, 2),
	"back_heap3": (0, -7),
	"back_hole": (0, 0),
	"back_kolb": (2, 0),
	"back_kolonna": (0, -2),
	"back_lab": (15, -10),
	"back_lcolor": (-8, -8),
	"back_light1": (0, 0),
	"back_light2": (0, 11),
	"back_light3": (0, 4),
	"back_light4": (0, 0),
	"back_light5": (0, 0),
	"back_light6": (0, 0),
	"back_light7": (0, 0),
	"back_med": (5, 0),
	"back_metal": (0, -30),
	"back_moss": (0, 29),
	"back_musor1": (0, 16),
	"back_musor2": (0, 16),
	"back_orudie": (0, 3),
	"back_paint": (0, 4),
	"back_pink": (0, -20),
	"back_pipe1": (0, 8),
	"back_pipes": (1, 0),
	"back_pipes2": (1, -17),
	"back_poster": (13, 0),
	"back_pult2": (0, 31),
	"back_qgraff": (0, 6),
	"back_rak": (0, 30),
	"back_remains1": (8, 17),
	"back_remains2": (345, -12),
	"back_skel": (-4, 5),
	"back_stbanner": (-5, 0),
	"back_stenka1": (2, 0),
	"back_stenka2": (2, 0),
	"back_stenka3": (2, -12),
	"back_stillage": (2, 0),
	"back_stlight1": (-53, -54),
	"back_stlight3": (-59, -41),
	"back_stlight4": (-89, 0),
	"back_stok": (1, 0),
	"back_stok2": (1, 0),
	"back_stree": (0, 6),
	"back_trava": (2, -2),
	"back_tree": (-10, 7),
	"back_unitaz": (5, 20),
	"back_unitazm": (5, 20),
	"back_vase": (-5, 7),
	"back_vent": (-5, -5),
	"back_vyt": (10, 0),
	"back_zavod1": (1, 0),
	"back_zavod2": (-5, -9),
	"back_zhaluzi": (-10, -14),
	"unnamed0": (-30, -30),
	"unnamed1": (-30, -30),
	"unnamed10": (-28, -12),
	"unnamed11": (-26, -9),
	"unnamed12": (-12, -12),
	"unnamed13": (-9, -9),
	"unnamed14": (-37, -12),
	"unnamed15": (-38, -7),
	"unnamed16": (-38, -4),
	"unnamed17": (-245, -132),
	"unnamed18": (-243, -151),
	"unnamed19": (-240, -141),
	"unnamed2": (-100, -81),
	"unnamed20": (-203, -187),
	"unnamed21": (-1, -6),
	"unnamed3": (-9, -40),
	"unnamed4": (-5, -40),
	"unnamed5": (-20, -60),
	"unnamed6": (-25, -42),
	"unnamed7": (-28, -28),
	"unnamed8": (-20, -60),
	"unnamed9": (-35, -5),
	"visaaa": (-80, -200),
	"visalarm": (-20, -70),
	"visalib1": (-20, -280),
	"visalib2": (-140, -40),
	"visammobox": (-20, -34),
	"visbarst": (-60, -45),
	"visbasechest": (-35, -40),
	"visbasedoor": (-20, -120),
	"visbed": (-59, -50),
	"visbigbox": (-50, -80),
	"visbigbox2": (-45, -110),
	"visbigexpl": (-46, -39),
	"visbigmed": (-40, -100),
	"visbookcase": (-40, -120),
	"visbox": (-30, -60),
	"visbox0": (-20, -40),
	"visbrspr": (-40, -27),
	"viscamp": (-120, -95),
	"viscase": (-20, -30),
	"visccup": (-20, -45),
	"vischest": (-30, -40),
	"viscouch": (-40, -39),
	"viscryocap": (-40, -114),
	"viscup": (-24, -80),
	"visdetonator": (-17, -32),
	"visdoor1": (-20, -80),
	"visdoor1a": (-20, -80),
	"visdoor1b": (-20, -80),
	"visdoor2": (-20, -80),
	"visdoor2a": (-30, -80),
	"visdoor3": (-20, -120),
	"visdoor4": (-20, -120),
	"visdoor_st1": (-124, -240),
	"visdoor_st2": (-124, -240),
	"visdoorboss": (-40, -120),
	"visdoorout": (-40, -120),
	"visdoorprob": (-40, -120),
	"visdsph": (-13, -33),
	"viselpanel": (-20, -40),
	"visenclcase": (-50, -44),
	"visenclchest": (-30, -40),
	"visencldoor": (-20, -120),
	"visenclpole": (-20, -120),
	"visexit": (-40, -120),
	"visexplbox": (-19, -33),
	"visfilecab": (-16, -80),
	"visfridge": (-20, -68),
	"visgenerat": (-100, -161),
	"visgrate": (-20, -120),
	"vishatch1": (-40, -40),
	"vishatch2": (-40, -40),
	"vishgrate": (-60, -40),
	"vishimlab": (-40, -95),
	"visinbasedoor": (-48, -117),
	"visindoor1": (-26, -85),
	"visindoor2": (-26, -85),
	"visindoor3": (-28, -58),
	"visindoor4": (-26, -85),
	"visinencldoor": (-48, -117),
	"visinstdoor": (-51, -117),
	"visinstr1": (-37, -51),
	"visinstr2": (-36, -37),
	"visknop1": (-12, -40),
	"visknop2": (-20, -40),
	"visknop3": (-12, -40),
	"visknop4": (-12, -40),
	"vislocker": (-40, -119),
	"vislongtable": (-58, -40),
	"vismcrate1": (-30, -60),
	"vismcrate2": (-32, -64),
	"vismcrate3": (-35, -70),
	"vismcrate4": (-35, -70),
	"vismcrate5": (-50, -88),
	"vismedbox": (-19, -38),
	"vismoln1": (-6, -120),
	"visniche1": (-40, -100),
	"visp_luna": (-60, -138),
	"vispcloud1": (-250, -250),
	"visplatform1": (-60, -40),
	"visplatform2": (-60, -40),
	"visplatform3": (-60, -40),
	"visradbarrel": (-20, -57),
	"visradbigbarrel": (-39, -120),
	"visreactor": (-160, -720),
	"visrobocell": (-40, -80),
	"vissafe": (-30, -60),
	"visseptum": (-20, -120),
	"vissmotr": (-100, -70),
	"visstand": (-80, -120),
	"visstdoor": (-20, -120),
	"visstove": (-20, -45),
	"vistable": (-31, -40),
	"vistable1": (-34, -40),
	"vistable2": (-33, -40),
	"vistable3": (-34, -60),
	"vistable4": (-40, -40),
	"vistap": (-20, -55),
	"vistcloud1": (-250, -250),
	"visterm": (-20, -40),
	"visterm1": (-20, -40),
	"visterm2": (-20, -40),
	"visterm3": (-20, -40),
	"vistermh": (-20, -40),
	"vistomb": (-24, -80),
	"vistrash": (-12, -44),
	"vistumba1": (-16, -39),
	"vistumba2": (-16, -36),
	"visvault": (-30, -40),
	"viswallcab": (-16, -38),
	"viswallsafe": (-20, -40),
	"viswcup": (-20, -40),
	"visweapbox": (-24, -78),
	"visweapcase": (-49, -38),
	"viswindow1": (-20, -80),
	"viswindow2": (-20, -80),
	"viswmap": (-80, -120),
	"viswoodbox": (-30, -60),
	"viswork": (-40, -55)
}

# empty images
obj_txt_blacklist = [
	"back_exit_light_0",
	"back_light1_1_l",
	"back_light2_2_l",
	"back_light3_1_l",
	"back_light4_1_l",
	"back_light5_2_l",
	"back_light6_2_l",
	"back_light7_2_l",
	"back_stlight1_2_l",
	"back_stlight3_2_l",
	"back_stlight4_2_l",
	"visalib1_1",
	"visalib1_2",
	"visalib2_1",
	"visalib2_2",
	"visplatform1_1",
	"visplatform1_2",
	"visplatform2_1",
	"visplatform2_2",
	"visplatform3_1",
	"visplatform3_2",

	# holes are kinda annoying.
	# first, "back_hole" is defined one time as standalone objects, second time as a combined object with 5 variants.
	# we want to keep the combined one.
	# second, in case of holes defined as combined object (with variants), the exported textures are sometimes weirdly
	# trimmed and so we want to use textures from standalone objects instead.
	"back_hole1_0",
	"back_hole2_0",
	"back_hole3_0",
	"back_hole4_0",
	"back_hole5_0",
	"back_hole1_0_e",
	"back_hole2_0_e",
	"back_hole3_0_e",
	"back_hole4_0_e",
	"back_hole5_0_e",
]

# these objects will be processed manually, with the help of pad_images.py
obj_copy_blacklist = [
	"back_hole",
	"back_chole",
]

objs_add_blend_flag = [
	"back_hole"
]

hole_ids = [
	"back_awindow",
	"back_bwindow",
	"back_chole",
	"back_cwindow",
	"back_enclwindow",
	"back_enil",
	"back_fwindow",
	"back_hole",
	"back_swindow",
]
