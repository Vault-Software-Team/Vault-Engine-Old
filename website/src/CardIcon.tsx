import React from "react";

// props
interface Props {
	children?: React.ReactNode;
	headerName: string;
	body: string;
	iconClass: string;
}

function CardIcon({
	children,
	headerName,
	iconClass,
	body,
}: Props): JSX.Element {
	return (
		<div className="card">
			<div className="card-header">
				<i className={iconClass} style={{ fontSize: "170px" }} />
				<h1>{headerName}</h1>
				<h3>{body}</h3>
			</div>
		</div>
	);
}

export default CardIcon;
