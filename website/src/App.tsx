import React from "react";
import Card from "./Card";
import CardIcon from "./CardIcon";
import Renderer from "./Renderer";

function App() {
	const previews = [
		"./prev1.png",
		"./prev2.png",
		"./prev3.png",
		"./prev4.png",
	];

	// get random index for previews
	const randomIndex = Math.floor(Math.random() * previews.length);

	return (
		<div className="App">
			<div className="navBar">
				<img src="logo2.png" width={60}></img>
				<a>
					<i className="fa-duotone fa-book"></i> Documentations
				</a>
				<a>
					<i className="fa-solid fa-newspaper"></i> News
				</a>
				<a>
					<i className="fa-solid fa-message"></i> Forums
				</a>
			</div>
			<br />
			<br />
			<br />
			<br />

			<div
				className="showcase"
				style={{
					backgroundImage: `url(${previews[randomIndex]})`,
				}}
			>
				<img src="logo1.png" width={400}></img>
				<h1
					style={{
						textAlign: "center",
					}}
				>
					<span
						style={{
							fontSize: "40px",
						}}
					>
						Static Engine
					</span>
					<br />
					Open Source 2D and 3D Game Engine
				</h1>
			</div>
			<div className="cards">
				<Card
					headerName="Lua Scripts"
					body="Write scripts using Lua"
					image="./Lua-Logo.svg"
				></Card>
				<Card
					headerName="2D & 3D Physics"
					body="Static Engine comes with 2D (Box2D) and 3D (Bullet) Physics!"
					image="./bullet.png"
				></Card>
				<CardIcon
					headerName="Make levels using the Editor"
					body="Static Engine comes with a level editor, "
					iconClass="fa-solid fa-pen"
				></CardIcon>
			</div>

			<div className="needHelp">
				<h1>Need Help?</h1>
				<h2
					style={{
						marginBottom: "-10px",
					}}
				>
					Use our{" "}
					<a
						href="#"
						style={{
							color: "rgb(255, 49, 49)",
						}}
					>
						forums
					</a>
					!
				</h2>

				<h2>
					or join our{" "}
					<a
						href="#"
						style={{
							color: "rgb(255, 49, 49)",
						}}
					>
						discord server
					</a>
					!
				</h2>
			</div>
		</div>
	);
}

export default App;
