from consts import *
from common import BackObject

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
	"hole_0_h.png": (13, 9),
	"hole_1_h.png": (18, 12),
	"hole_2_h.png": (0, 18),
	"hole_3_h.png": (14, 6),
	"hole_4_h.png": (8, 13),
	"skel_1.png": (0, 7),
	"skel_2.png": (0, 10),
}

# back objects which have a "*_l" texture. need to mark them manually as not all have the "lon"/"loff" attributes,
# and there's no other known way of identifying light objs.
obj_back_lights = [
	"bar",
	"exit_light",
	"light1",
	"light2",
	"light3",
	"light4",
	"light5",
	"light6",
	"light7",
	"shadow",
	"stlight1",
	"stlight3",
	"stlight4",
]

# back objects which have a single empty or 1x1 px "*_t" texture which should be ignored
obj_back_light_skip_txt_t = [
	"exit_light",
	"shadow",
]

# back light objects with "loff" attribute incorrectly defined or not defined at all
obj_back_light_loff_override = {
	"light5": 3,
	"light6": 3,
	"light7": 3,
	"stlight1": 3,
}

obj_back_last_main_txt_missing = [
	"bar",
	"light5",
	"light6",
	"light7",
]

# ignore "blend" attribute for these back objects
obj_back_blend_blacklist = [
	"blood1",
	"blood2",
]

# objects which are referenced by variants, but not defined in AllData.
# values are (object_id, light_cnt)
obj_back_missing_parent_variants = {
	BackObject(id="lcolor", has_light_txt=True),
}

# set this manually, based on extract_svg_offsets.py
back_obj_offsets = {
	"awindow": (-3, 0),
	"barrow": (6, 14),
	"battery": (1, 0),
	"bio": (0, 6),
	"blood1": (-8, 0),
	"blood2": (0, 0),
	"books": (2, 0),
	"bvent": (-5, 0),
	"celest": (40, 0),
	"chert": (-20, -20),
	"clock": (-5, -3),
	"door": (12, 20),
	"electro": (2, 0),
	"enclave": (-10, 0),
	"fuse": (-2, 0),
	"fwindow": (-5, 0),
	"generator": (1, 0),
	"h109": (20, 20),
	"heap1": (0, -2),
	"heap2": (0, 2),
	"heap3": (0, -7),
	"hole": (0, 0),
	"kolb": (2, 0),
	"kolonna": (0, -2),
	"lab": (15, -10),
	"lcolor": (-8, -8),
	"light1": (0, 0),
	"light2": (0, 11),
	"light3": (0, 4),
	"light4": (0, 0),
	"light5": (0, 0),
	"light6": (0, 0),
	"light7": (0, 0),
	"med": (5, 0),
	"metal": (0, -30),
	"moss": (0, 29),
	"musor1": (0, 16),
	"musor2": (0, 16),
	"orudie": (0, 3),
	"paint": (0, 4),
	"pink": (0, -20),
	"pipe1": (0, 8),
	"pipes": (1, 0),
	"pipes2": (1, -17),
	"poster": (13, 0),
	"pult2": (0, 31),
	"qgraff": (0, 6),
	"rak": (0, 30),
	"remains1": (8, 17),
	"remains2": (345, -12),
	"skel": (-4, 2),
	"stbanner": (-5, 0),
	"stenka1": (2, 0),
	"stenka2": (2, 0),
	"stenka3": (2, -12),
	"stillage": (2, 0),
	"stlight1": (0, 0),
	"stlight3": (0, 0),
	"stlight4": (0, 0),
	"stok": (1, 0),
	"stok2": (1, 0),
	"stree": (0, 6),
	"trava": (2, -2),
	"tree": (-10, 7),
	"unitaz": (5, 20),
	"unitazm": (5, 20),
	"vase": (-5, 9), # TODO fix obj order in MoI (0, 0, 0) (must be behind back_celest)
	"vent": (-5, -5),
	"vyt": (10, 0),
	"zavod1": (1, 0),
	"zavod2": (-5, -9),
	"zhaluzi": (-10, -14),
}

# generated manually, no sense wasting life figuring out where to get this offsets from
back_obj_lights_offsets = {
	"light1": (-111, -111),
	"light2": (-117, -73),
	"light3": (-114, -75),
	"light4": (-88, 0),
	"light5": (-53, -53),
	"light6": (-56, -56),
	"light7": (-35, -35),
	"stlight1": (-111, -111),
	"stlight3": (-114, -79),
	"stlight4": (-88, 0),
}

# set this manually, based on extract_svg_offsets.py
obj_offsets = {
	"aaa": (-80, -200),
	"alarm": (-20, -70),
	"alib1": (-20, -280),
	"alib2": (-140, -40),
	"ammobox": (-20, -34),
	"barst": (-60, -45),
	"basechest": (-35, -40),
	"basedoor": (-20, -120),
	"bed": (-59, -50),
	"bigbox": (-50, -80),
	"bigbox2": (-45, -110),
	"bigexpl": (-46, -39),
	"bigmed": (-40, -100),
	"bookcase": (-40, -120),
	"box": (-30, -60),
	"box0": (-20, -40),
	"brspr": (-40, -27),
	"camp": (-120, -95),
	"case": (-20, -30),
	"ccup": (-20, -45),
	"chest": (-30, -40),
	"couch": (-40, -39),
	"cryocap": (-40, -114),
	"cup": (-24, -80),
	"detonator": (-17, -32),
	"door1": (-20, -80),
	"door1a": (-20, -80),
	"door1b": (-20, -80),
	"door2": (-20, -80),
	"door2a": (-30, -80),
	"door3": (-20, -120),
	"door4": (-20, -120),
	"door_st1": (-124, -240),
	"door_st2": (-124, -240),
	"doorboss": (-40, -120),
	"doorout": (-40, -120),
	"doorprob": (-40, -120),
	"dsph": (-13, -33),
	"elpanel": (-20, -40),
	"enclcase": (-50, -44),
	"enclchest": (-30, -40),
	"encldoor": (-20, -120),
	"enclpole": (-20, -120),
	"exit": (-40, -120),
	"explbox": (-19, -33),
	"filecab": (-16, -80),
	"fridge": (-20, -68),
	"generat": (-100, -161),
	"grate": (-20, -120),
	"hatch1": (-40, -40),
	"hatch2": (-40, -40),
	"hgrate": (-60, -40),
	"himlab": (-40, -95),
	"inbasedoor": (-48, -117),
	"indoor1": (-26, -85),
	"indoor2": (-26, -85),
	"indoor3": (-28, -58),
	"indoor4": (-26, -85),
	"inencldoor": (-48, -117),
	"instdoor": (-51, -117),
	"instr1": (-37, -51),
	"instr2": (-36, -37),
	"knop1": (-12, -40),
	"knop2": (-20, -40),
	"knop3": (-12, -40),
	"knop4": (-12, -40),
	"locker": (-40, -119),
	"longtable": (-58, -40),
	"mcrate1": (-30, -60),
	"mcrate2": (-32, -64),
	"mcrate3": (-35, -70),
	"mcrate4": (-35, -70),
	"mcrate5": (-50, -88),
	"medbox": (-19, -38),
	"moln1": (-6, -120),
	"niche1": (-40, -100),
	"p_luna": (-60, -138),
	"pcloud1": (-250, -250),
	"platform1": (-60, -40),
	"platform2": (-60, -40),
	"platform3": (-60, -40),
	"radbarrel": (-20, -57),
	"radbigbarrel": (-39, -120),
	"reactor": (-160, -720),
	"robocell": (-40, -80),
	"safe": (-30, -60),
	"septum": (-20, -120),
	"smotr": (-100, -70),
	"stand": (-80, -120),
	"stdoor": (-20, -120),
	"stove": (-20, -45),
	"table": (-31, -40),
	"table1": (-34, -40),
	"table2": (-33, -40),
	"table3": (-34, -60),
	"table4": (-40, -40),
	"tap": (-20, -55),
	"tcloud1": (-250, -250),
	"term": (-20, -40),
	"term1": (-20, -40),
	"term2": (-20, -40),
	"term3": (-20, -40),
	"termh": (-20, -40),
	"tomb": (-24, -80),
	"trash": (-12, -44),
	"tumba1": (-16, -39),
	"tumba2": (-16, -36),
	"vault": (-30, -40),
	"wallcab": (-16, -38),
	"wallsafe": (-20, -40),
	"wcup": (-20, -40),
	"weapbox": (-24, -78),
	"weapcase": (-49, -38),
	"window1": (-20, -80),
	"window2": (-20, -80),
	"wmap": (-80, -120),
	"woodbox": (-30, -60),
	"work": (-40, -55)
}

# these back objects should be processed manually, with the help of pad_images.py and dark magic.
# this is required, because textures exported from swf are weirdly trimmed.
back_obj_copy_blacklist = [
	"hole",
	"chole",
	"skel",
]

# "named" variants which are missing the "tid" and "fr" attributes
back_obj_missing_tid_fr = [
	"hole1",
	"hole2",
	"hole3",
	"hole4",
	"hole5",
]

hole_ids = [
	"awindow",
	"bwindow",
	"chole",
	"cwindow",
	"enclwindow",
	"enil",
	"fwindow",
	"hole",
	"swindow",
]

# for fixing room geometry
extra_cell_solids = {
	"fort_rocks": {
		(7, 1, 0): {
			(1, 0): 'O',
			(2, 0): 'O',
		},
		(9, 0, 0): {
			(47, 5): 'O',
		},
	},
	"stable_pi_entrance": {
		(4, 0, 0): {
			(47, 0): 'C',
			(46, 0): 'D',
			(45, 0): 'D',
			(44, 0): 'D',
			(43, 0): 'D',
			(42, 0): 'D',
			(41, 0): 'D',
		},
	},
	"mas": {
		(1, 8, 0): {
			(47, 21): 'C',
			(47, 22): 'C',
			(47, 23): 'C',
			(47, 24): 'C',
			(46, 21): 'C',
			(46, 22): 'C',
			(46, 23): 'C',
			(46, 24): 'C',
		},
	},
}

back_obj_variant_mapping = {
	"stwindow2": ("stwindow", 2), # no error here, stwindow2 has fr='3'
	"chole1": ("chole", 0),
	"chole2": ("chole", 1),
	"chole3": ("chole", 2),
	"chole4": ("chole", 3),
	"chole5": ("chole", 4),
	"chole6": ("chole", 5),
	"hole1": ("hole", 0),
	"hole2": ("hole", 1),
	"hole3": ("hole", 2),
	"hole4": ("hole", 3),
	"hole5": ("hole", 4),
	"lwhite": ("lcolor", 0),
	"lred": ("lcolor", 1),
	"lorange": ("lcolor", 2),
	"lyellow": ("lcolor", 3),
	"lacid": ("lcolor", 4),
	"lgreen": ("lcolor", 5),
	"lcyan": ("lcolor", 6),
	"lblue": ("lcolor", 7),
	"lpurple": ("lcolor", 8),
	"lpink": ("lcolor", 9),
}
