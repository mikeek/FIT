<?php 

#CHA:xkozub03

define("ERR_OK", 0);
define("ERR_PARAMS", 1);
define("ERR_INPUT", 2);
define("ERR_OUTPUT", 3);


iconv_set_encoding("internal_encoding", "UTF-8");

define("CODE", 0);
define("SLASH", 1);
define("IN_STR", 2);
define("IN_MACRO", 3);
define("LINE_COMMENT", 4);
define("MULTI_COMMENT", 5);
define("MULTI_COMMENT_STAR", 6);

/** 
 * Tisk napovedy.
 */
function usage()
{
	echo "\n";
	echo "Skript pro analyzu hlavickovych souboru jazyka C (pripona .h) podle standardu\n";
	echo "ISO C99, ktery vytvori databazi nalezenych funkci v techto souborech.\n";
	echo "\n";
	echo "Pouziti: php -d base_dir=\"\" proj.php  [--help] [--input=fileordir]\n";
	echo "                                      [--output=filename] [pretty-xml[=k]]\n";
	echo "                                      [--no-inline] [--max-par=n]\n";
	echo "                                      [--remove-whitespace]\n";
	echo "--help               Vypis napovedy\n";
	echo "--input=fileordir    Vstupni soubor/adresar se zdrojovym kodem v jazyce C.\n";
	echo "                     Pri zadani adresare jsou analyzovany vsechny soubory *.h\n";
	echo "                     v adresari i vsech jeho vsech podadresarich (default=./)\n";
	echo "--output=filename    Vystupni soubor\n";
	echo "--pretty-xml=k       Zformatovani XML (kazde zanoreni odsazeno o k mezer\n";
	echo "                     (defaultne k = 4), odradkovani root elementu od hlavicky)\n";
	echo "--no-inline          Preskoceni inline funkci\n";
	echo "--max-par=n          Zpracuji se jen funkce s maximalne n parametry\n";
	echo "--remove-whitespace  V rettype a type se nahradi bile znaky jine nez mezera\n";
	echo "                     mezerou a prebytecne mezery budou odstraneny\n";
	echo "\n";
}

/**
 * Kontrola na vicenasobne uvedeni parametru.
 * 
 * @param $opts Nactene parametry
 * @param $value Kontrolovany parametr
 */
function check_mult_opts(array $opts, $value)
{
	if (is_array($opts[$value])) {
		fwrite(STDERR, "Vicenasobne pouziti '$value'!\n");
		exit(ERR_PARAMS);
	}
}

/**
 * Kontrola, ze nebyla zadana hodnota parametru (jelikoz getopt funguje dosti zvlastne)
 * 
 * @param $value Hodnota parametru
 * @param $opt Nazev parametru
 */
function check_false_val($value, $opt)
{
	if ($value !== false) {
		fwrite(STDERR, "Neocekavana hodnota pro '$opt'!\n");
		exit(ERR_PARAMS);
	}
}

/**
 * Test spravneho otevreni souboru
 * 
 * @param $file Soubor
 * @param $name Nazev souboru
 * @param $err_code Chybovy kod
 */
function check_fopen($file, $name, $err_code)
{
	if (!$file) {
		fwrite(STDERR, "Nelze otevrit soubor '$name'!\n");
		exit($err_code);
	}
}


/**
 * Nalezeni vsech souboru v adresari a jeho podadresarich
 * 
 * @param $arr Vystupni pole s nazvy souboru
 * @param $path Cesta k zakladnimu adresari
 */
function find_files(&$arr, $path) 
{
	$iter = new RecursiveDirectoryIterator($path);
	foreach(new RecursiveIteratorIterator($iter) as $filename => $file) {
		if (preg_match("/\.h$/", $filename)) {
			$arr[] = $filename;
		}
	}
}

/**
 * Odstrani z textu komentare, stringy a makra
 * 
 * @param $text vstupni text
 * @return vystupni text
 */
function remove_unnecessary($text)
{
	$text = preg_replace('/\\\(\n)/u', "", $text);
	$textarr = preg_split('//u', $text);
	$res = "";
	$start = 0;
	$i = 0;
	$state = CODE;
	$backstate = CODE;
	$skip = false;
	$len = count($textarr);
	while ($i < $len) {
		$char = $textarr[$i];
		switch ($state) {
		case CODE:
			if ($char === "/") {
				$state = SLASH;
				$backstate = CODE;
			} else if ($char === '"' || $char === "'") {
				$res .= iconv_substr($text, $start, $i - $start - 1);
				$ending = $char;
				$state = IN_STR;
			} else if ($char === "#") {
				$res .= iconv_substr($text, $start, $i - $start - 1);
				$state = IN_MACRO;
			} else if ($i === $len - 1) {
				$res .= iconv_substr($text, $start, $i - $start);
			}
			break;
		case SLASH:
			if ($char === "/") {
				if ($backstate === CODE) {
					$res .= iconv_substr($text, $start, $i - $start - 2);
				}
				$state = LINE_COMMENT;
			} else if ($char === "*") {
				if ($backstate === CODE) {
					$res .= iconv_substr($text, $start, $i - $start - 2);
				}
				$state = MULTI_COMMENT;
			} else {
				$state = $backstate;
			}
			break;
		case IN_STR:
			if ($skip) {
				$skip = false;
			} else if ($char === $ending) {
				$start = $i;
				$state = CODE;
			} else if ($char === '\\') {
				$skip = true;
			}
			break;
		case IN_MACRO:
			if ($char === "\n") {
				$start = $i;
				$state = CODE;
			} else if ($char === "/") {
				$state = SLASH;
				$backstate = IN_MACRO;
			}
			break;
		case LINE_COMMENT:
			if ($char === "\n") {
				$start = $i;
				$state = CODE;
			}
			break;
		case MULTI_COMMENT:
			if ($char === "*") {
				$state = MULTI_COMMENT_STAR;
			}
			break;
		case MULTI_COMMENT_STAR:
			if ($char === "/") {
				$start = $i;
				$state = $backstate;
			} else if ($char !== "*") {
				$state = MULTI_COMMENT;
			}
			break;
		}
		$i++;
	}
	return $res;
	
}

/* Nastaveni defaultnich flagu a hodnot */
$pretty = false;
$noinline = false;
$nodupl = false;
$maxpar = false;
$rem_white = false;

$maxpar_n = 0;
$indent = 4;
$output = STDOUT;
$att_dir = "./";

/* Prijimane parametry */
$longops = array(
	"help::",
	"input:",
	"output:",
	"max-par:",
	"pretty-xml::",
	"no-inline::",
	"no-duplicates::",
	"remove-whitespace::"
);

/* Nacteni zadanych parametru a test na chybu */
$options = getopt(NULL, $longops);

if ($options === false) {
	fwrite(STDERR, "Error while parsing arguments!\n");
	exit(ERR_PARAMS);
}

foreach(array_keys($options) as $opt) {
	check_mult_opts($options, $opt);
	switch ($opt) {
		case "help":
			check_false_val($options[$opt], $opt);
			/* S napovedou nesmi byt zadan jiny parametr */
			if (count($options) === 1) {
				usage();
				exit(ERR_OK);
			} else {
				fwrite(STDERR, "S '--help' nesmi byt zadany jiny parametr!\n");
				exit(ERR_PARAMS);
			}
			break;
			
		case "input":
			/* Adresar nebo soubor? */
			if (is_dir($options[$opt])) {
				find_files($input, $options[$opt] . "/");
				$att_dir = $options[$opt];
				if ($att_dir[strlen($att_dir) - 1] !== "/") {
					$att_dir .= "/";
				}
				$att_dir = preg_replace('/\/\/+$/', "/", $att_dir);
			} else {
				$input[0] = $options[$opt];
				$att_dir = "";
			}
			break;
			
		case "output":
			$output_file = $options[$opt];
			break;
		
		case "pretty-xml":
			$pretty = true;
			if ($options[$opt] !== false) {
				$indent = $options["pretty-xml"];
			}
			break;
			
		case "no-inline":
			check_false_val($options[$opt], $opt);
			$noinline = true;
			break;
		
		case "no-duplicates":
			check_false_val($options[$opt], $opt);
			$nodupl = true;
			break;
		
		case "max-par":
			$maxpar = true;
			$maxpar_n = $options[$opt];
			break;
		
		case "remove-whitespace":
			check_false_val($options[$opt], $opt);
			$rem_white = true;
			break;
		
		default:
			fwrite(STDERR, "Undefined option!\n");
			exit(ERR_PARAMS);
			break;
	}
}

/* Kontrola prebytecnych parametru */
if ($argc !== count($options) + 1) {
	fwrite(STDERR, "Nezname parametry!\n");
	exit(ERR_PARAMS);
}

/* Otevreni vystupniho souboru */
if (isset($output_file)) {
	@$output= fopen($output_file, 'w');
	check_fopen($output, $options[$opt], ERR_OUTPUT);
}

if (!isset($input)) {
	find_files($input, "./");
}

/* Vytvoreni XML struktury */
$domtree = new DOMDocument('1.0', 'utf-8');

$root = $domtree->createElement("functions");
$root->setAttribute("dir", $att_dir);

$root = $domtree->appendChild($root);

$identif = "[a-zA-Z_][a-zA-Z_0-9]*";
$func_patt = '/(?<rettype>(?:\s*'.$identif.'[\s\*]+)+)(?<name>'.$identif.')\s*\((?<args>[^\)]*)\)\s*[\{;]/u';
$par_name_patt = '/(?<type>(?:'.$identif.'(?:[\s\*]|\[[^\]]*\])+)*?[\s\*]*)(?<name>'.$identif.')?\s*$/u';
$inline_patt = '/(^|\s+)inline\s/u';
$var_args_patt = '/\.\.\.\s*/u';

$par_keyword = '/^(const|int|double|char|float|signed|unsigned|short|long)$/u';
$any_f = false;
foreach($input as $file) {
	if ($att_dir !== "") {
		$att_file = ltrim(substr($file, strlen($att_dir)), "/");
	} else {
		$att_file = $file;
	}
	
	$text = file_get_contents($file);
	if ($text === false) {
		fclose($output);
		fwrite(STDERR, "Error while opening $file!\n");
		exit(ERR_INPUT);
	}
	
	$text = remove_unnecessary($text);
	$functions = array();
	$duplicates = array();
	$matches = array();
	
	if (preg_match_all($func_patt, $text, $functions)) {
		$num_match = count($functions[0]);
		for ($i = 0; $i < $num_match; ++$i) { 
			$att_rettype = trim($functions["rettype"][$i]);
			$att_name = $functions["name"][$i];
			
			if ($noinline && preg_match($inline_patt, $att_rettype)) {
				continue;
			}
			
			if ($nodupl) {
				if (array_key_exists($att_name, $duplicates)) {
					continue;
				} else {
					$duplicates[$att_name] = 1;
				}
			}
			
			$args = $functions["args"][$i];
			$att_varargs = preg_match($var_args_patt, $args) ? "yes" : "no";
			
			if ($rem_white) {
				$att_rettype = preg_replace("/\s+/", " ", $att_rettype);
				$att_rettype = preg_replace("/ \*/", "*", $att_rettype);
			}
			
			$xmlfunc = $domtree->createElement("function");
			$any_p = false;
			$par_num = 0;
			if ($args !== "" && trim($args) !== "void") {
				$args_split = explode(",", $args);
				foreach($args_split as $par) {
					$par = trim($par);
					if ($par === "...") {
						continue;
					}
					$par_num++;
					preg_match($par_name_patt, $par, $matches);
					if (!array_key_exists("name", $matches) || $matches["type"] === "") {
						$par_name = "";
						$par_type = $par;
					} else {
						$par_name = $matches["name"];
						$par_type = trim($matches["type"]);
					}
					
					if (preg_match($par_keyword, $par_name)) {
						$par_type = $par;
					}
					
					if ($rem_white) {
						$par_type = preg_replace("/\s+/", " ", $par_type);
						$par_type = preg_replace("/\s*\*\s*/", "*", $par_type);
					}
					$xmlpar = $domtree->createElement("param");
					$xmlpar->setAttribute("number", $par_num);
					$xmlpar->setAttribute("type", $par_type);
					$xmlpar = $xmlfunc->appendChild($xmlpar);
					$any_p = true;
				} 
			}
			if ($maxpar && $par_num > $maxpar_n) {
				continue;
			} 
			if (!$any_p) {
				$xmlfunc->appendChild($domtree->createTextNode(''));
			}
			$xmlfunc->setAttribute("file", $att_file);
			$xmlfunc->setAttribute("name", $att_name);
			$xmlfunc->setAttribute("varargs", $att_varargs);
			$xmlfunc->setAttribute("rettype", $att_rettype);
			
			$xmlfunc = $root->appendChild($xmlfunc);
			$any_f = true;
		}
		
	}
}
if (!$any_f) {
	$root->appendChild($domtree->createTextNode(''));
}
$xmlstr = $domtree->saveXML();

if ($pretty) {
	$xmlstr = str_replace("<", "\n<", $xmlstr);
	$xmlstr = str_replace("\n<?", "<?", $xmlstr);
	$xmlstr = str_replace("\n\n<f", "\n<f", $xmlstr);
	$xmlstr = str_replace("<function ", str_repeat(" ", $indent) . "<function ", $xmlstr);
	$xmlstr = str_replace("</function>", str_repeat(" ", $indent) . "</function>", $xmlstr);
	$xmlstr = str_replace("<param ", str_repeat(" ", $indent*2) . "<param ", $xmlstr);
} else {
	$xmlstr = str_replace("\n", "", $xmlstr);
	$xmlstr .= "\n";
}

$xmlstr = str_replace('&#10;', "\n", str_replace('&#9;', "\t", str_replace('"/>', '" />', $xmlstr)));

fwrite($output, $xmlstr);

fclose($output);
exit(ERR_OK);
?>