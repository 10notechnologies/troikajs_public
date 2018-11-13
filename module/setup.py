from distutils.core import setup, Extension

module1 = Extension(
    'troika_ai',
    sources = [ 'troika_ai.cpp',
                'troika_common.cpp',
                'troika_get_bid.cpp',
                'troika_get_pass_cards.cpp',
                'troika_get_play_cards.cpp',
                'troika_get_trump.cpp']
)

setup ( 
    name = 'troika_ai',
    version = '1.0',
    description = 'This is the AI for Troika',
    ext_modules = [module1]
)



