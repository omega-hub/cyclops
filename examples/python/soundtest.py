se = getSoundEnvironment()
music = se.loadSoundFromFile('music', 'Users/evldemo/sounds/music/filmic.mp3')
sample = se.loadSoundFromFile('music', 'Users/evldemo/sounds/menu_sounds/menu_load.wav')

simusic = SoundInstance(music)
sisample = SoundInstance(sample)

simusic.play()
