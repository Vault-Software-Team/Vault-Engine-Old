fn main() {
    let str = String::from("windows");
    // let str = String::from("linux");

    // println!("cargo:rustc-link-search=../win_libs");
    // println!("cargo:rustc-flags=-l dylib=stdc++ -lsndfile.dll -lBulletDynamics.dll -lBulletCollision.dll -lLinearMath.dll -lopenal.dll -lmono-2.0.dll -lglfw3dll -lstdc++fs -lluajit-5.1 -lbox2d -lassimp.dll -lfreetype.dll -lSDL2.dll -lSDL2_mixer.dll -ldiscord-rpc -ltinyxml2")
    // match str.as_str() {
    //     "windows" => {
    //         println!("cargo:rustc-link-search=../win_libs");
    //         println!("cargo:rustc-flags=-l dylib=stdc++ -lsndfile.dll -lBulletDynamics.dll -lBulletCollision.dll -lLinearMath.dll -lopenal.dll -lmono-2.0.dll -lglfw3dll -lstdc++fs -lluajit-5.1 -lbox2d -lassimp.dll -lfreetype.dll -lSDL2.dll -lSDL2_mixer.dll -ldiscord-rpc -ltinyxml2")
    //     }
    //     _ => {}
    // }
}
