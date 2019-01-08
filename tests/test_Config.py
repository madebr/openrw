#!/usr/bin/env python3

import configparser
import openrw
import os
import tempfile
import unittest

from test_Globals import OutputRedirector


class TestArgParserMethods(unittest.TestCase):
    def setUp(self):
        self.redirector = OutputRedirector()

    def test_initial(self):
        openrw.config.RWArgumentParser()

    def test_printHelp(self):
        argparser = openrw.config.RWArgumentParser()
        with self.redirector.redirect():
            argparser.print_help()
        self.assertTrue('--newgame' in self.redirector.stdout_text())
        self.assertEqual(len(self.redirector.stderr_text()), 0)

    def test_parse_arguments_nothing(self):
        argparser = openrw.config.RWArgumentParser()
        with self.assertRaises(ValueError):
            argparser.parse_arguments([])

    def test_parse_arguments_noarguments(self):
        argparser = openrw.config.RWArgumentParser()
        args = argparser.parse_arguments([''])
        self.assertIsNotNone(args)

    def test_parse_arguments_nopositionals(self):
        argparser = openrw.config.RWArgumentParser()
        with self.redirector.redirect():
            with self.assertRaises(ValueError):
                args = argparser.parse_arguments(['', '5'])
        self.assertEqual(len(self.redirector.stdout_text()), 0)
        self.assertNotEqual(len(self.redirector.stderr_text()), 0)

    def test_parse_arguments_unkonwn(self):
        argparser = openrw.config.RWArgumentParser()
        with self.redirector.redirect():
            with self.assertRaises(ValueError):
                args = argparser.parse_arguments(['', '--unknownArgument'])
        self.assertEqual(len(self.redirector.stdout_text()), 0)
        self.assertNotEqual(len(self.redirector.stderr_text()), 0)

    def test_parse_arguments_configPath(self):
        argparser = openrw.config.RWArgumentParser()

        args = argparser.parse_arguments([''])
        self.assertIsNotNone(args)
        self.assertIsNone(args.configPath)

        args = argparser.parse_arguments(['', '-c', 'place'])
        self.assertIsNotNone(args)
        self.assertEqual(args.configPath, 'place')

    def test_parse_arguments_width(self):
        argparser = openrw.config.RWArgumentParser()

        args = argparser.parse_arguments([''])
        self.assertIsNotNone(args)
        self.assertIsNone(args.width)

        args = argparser.parse_arguments(['', '--width', '400'])
        self.assertIsNotNone(args)
        self.assertEqual(args.width, 400)

    def test_parse_arguments_nopython(self):
        argparser = openrw.config.RWArgumentParser()

        args = argparser.parse_arguments([''])
        self.assertIsNotNone(args)
        self.assertIsNone(args.nopython)

        args = argparser.parse_arguments(['', '--nopython'])
        self.assertIsNotNone(args)
        self.assertTrue(args.nopython)


class TestConfParserMethods(unittest.TestCase):
    @staticmethod
    def create_config_file(overrides=None):
        config = configparser.ConfigParser()
        config['game'] = {
            'path': '\t/dev/test  \t \r\n',
            'language': '      american ;american english french german italian spanish.',
            'hud_scale': '2.0\t;HUD scale',
        }
        config['input'] = {'invert_y': '1 #values != 0 enable input inversion. Optional.'}
        if overrides is not None:
            for section, section_contents in overrides.items():
                config[section] = section_contents
        f = tempfile.NamedTemporaryFile(mode='w')
        config.write(f.file)
        f.file.flush()
        return f

    def test_config_path(self):
        path = openrw.config.RWConfigParser.default_config_path
        self.assertIsNotNone(path)
        self.assertTrue(os.path.isdir(path))

    def test_initial(self):
        openrw.config.RWConfigParser()

    def test_valid_file(self):
        f = self.create_config_file()
        cfg = openrw.config.RWConfigParser()
        layer, extras = cfg.load(f.name)
        self.assertIsNotNone(layer)
        self.assertEqual(extras, {})

        self.assertEqual(layer.gamedataPath, '/dev/test')
        self.assertEqual(layer.hudScale, 2.0)
        self.assertEqual(layer.gameLanguage, 'american')
        self.assertTrue(layer.invertY)

    def test_valid_file_extra(self):
        extras_in = {'extra': {'a': 'a1', 'b': 'b2'}, 'extra2': {'c': 'c2'}}
        extras_ref = {'{}.{}'.format(section, key): value for section, section_contents in extras_in.items() for key, value in section_contents.items() }
        f = self.create_config_file(extras_in)
        cfg = openrw.config.RWConfigParser()
        layer, extras = cfg.load(f.name)
        self.assertIsNotNone(layer)
        self.assertEqual(extras, extras_ref)

    def test_non_exsting_file(self):
        cfg = openrw.config.RWConfigParser()
        f = self.create_config_file()
        name = f.name
        del f
        self.assertFalse(os.path.exists(name))
        with self.assertRaises(ValueError):
            cfg.load(name)


class TestConfigLayerMethods(unittest.TestCase):
    def test_layer(self):
        openrw.config.RWConfigLayer()
    def test_default(self):
        default = openrw.config.RWConfigLayer.default()
        self.assertIsNotNone(default.width)
        self.assertIsNotNone(default.height)
        self.assertIsNone(default.gamedataPath)


class TestConfigMethods(unittest.TestCase):
    def test_config(self):
        cfg = openrw.config.RWConfig()
        self.assertNotEqual(cfg.missing_keys, [])
    def test_assign(self):
        cfg = openrw.config.RWConfig()
        cfg.layers[int(openrw.config.RWConfig.ARGUMENT)] = openrw.config.RWConfigLayer.default()
        self.assertNotEqual(cfg.missing_keys, ['game.gamedatapath'])
        cfg.layers[int(openrw.config.RWConfig.CONFIGFILE)].gamedataPath = openrw.config.RWConfigParser.default_config_path
        self.assertNotEqual(cfg.missing_keys, [])

if __name__ == '__main__':
    unittest.main()
