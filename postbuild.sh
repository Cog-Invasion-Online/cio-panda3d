cd built
echo Deleting unneeded files...
rm -rf bin
rm -rf direct/extensions
rm -rf etc
rm -rf models
rm -rf plugins
rm -rf python
rm LICENSE
rm ReleaseNotes

cd lib
rm libp3pystub.a

mv direct.a lib_panda3d_direct.a
mv core.a lib_panda3d_core.a
mv egg.a lib_panda3d_egg.a
mv fx.a lib_panda3d_fx.a
mv ode.a lib_panda3d_ode.a
mv physics.a lib_panda3d_physics.a
mv interrogatedb.a lib_panda3d_interrogatedb.a

cd ..
cd ..
