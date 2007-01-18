#ifndef _LOGCONTROL_H_
#define _LOGCONTROL_H_

class CLogControl
{
public:
			CLogControl();
			~CLogControl();

	bool	GetGSLoggingStatus();
	void	SetGSLoggingStatus(bool);

	bool	GetDMACLoggingStatus();
	void	SetDMACLoggingStatus(bool);

	bool	GetIPULoggingStatus();
	void	SetIPULoggingStatus(bool);

	bool	GetOSLoggingStatus();
	void	SetOSLoggingStatus(bool);

	bool	GetSIFLoggingStatus();
	void	SetSIFLoggingStatus(bool);

	bool	GetIOPLoggingStatus();
	void	SetIOPLoggingStatus(bool);

	bool	GetOSRecordingStatus();
	void	SetOSRecordingStatus(bool);

private:
	void	LoadConfig();
	void	SaveConfig();

	bool	m_nGSLogging;
	bool	m_nDMACLogging;
	bool	m_nIPULogging;
	bool	m_nOSLogging;
	bool	m_nSIFLogging;
	bool	m_nIOPLogging;
	bool	m_nOSRecording;
};

#endif
