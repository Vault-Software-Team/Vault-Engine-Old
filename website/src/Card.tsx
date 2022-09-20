import React from "react";

// props
interface Props {
	children?: React.ReactNode;
	headerName: string;
	image: string;
	body: string;
}

function Card({ children, headerName, image, body }: Props): JSX.Element {
	return (
		<div className="card">
			<div className="card-header">
				<img src={image} width="200px" />
				<h1>{headerName}</h1>
				<h3>{body}</h3>
			</div>
		</div>
	);
}

export default Card;
